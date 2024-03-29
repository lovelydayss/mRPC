#include "tcp_connection.h"
#include "abstract_codec.h"
#include "config.h"
#include "eventloop.h"
#include "fd_event.h"
#include "fd_event_pool.h"
#include "net_addr.h"
#include "rpc_dispatcher.h"
#include "tinypb_codec.h"
#include "tinypb_protocol.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

TcpConnection::TcpConnection(const EventLoop::s_ptr& eventloop, int fd,
                             const NetAddr::s_ptr& peer_addr,
                             const NetAddr::s_ptr& local_addr,
                             TcpConnectionType type)
    : m_eventloop(eventloop)
    , m_peer_addr(peer_addr)
    , m_fd(fd)
    , m_local_addr(local_addr)
    , m_connection_type(type) {

	m_in_buffer = std::make_shared<TcpBuffer>(
	    Config::GetGlobalConfig()->getConnectionBufferSize());
	m_out_buffer = std::make_shared<TcpBuffer>(
	    Config::GetGlobalConfig()->getConnectionBufferSize());

	m_fd_event = FdEventPool::GetGlobalFdEventPool()->getFdEvent(fd);
	m_fd_event->setNonBlock();
	m_fd_event->listen(FdEvent::IN_EVENT,
	                   std::bind(&TcpConnection::onRead, this));
	m_eventloop->addEpollEvent(m_fd_event);

	m_codec = std::make_shared<TinyPBCodec>();
}

void TcpConnection::onRead() {
	// 1. 从 socket 缓冲区，调用 系统的 read 函数读取字节 in_buffer 里面

	if (m_state != Connected) {
		ERRORFMTLOG("onRead error, client has already disconneced, addr[{}], "
		         "clientfd[{}]",
		         m_peer_addr->toString().c_str(), m_fd);
		return;
	}

	bool is_read_all = false;
	bool is_close = false;
	while (!is_read_all) {
		if (m_in_buffer->writeAble() == 0)
			m_in_buffer->resizeBuffer(2 * m_in_buffer->getBufferSize());

		int read_count = m_in_buffer->writeAble();
		int write_index = m_in_buffer->writeIndex();

		int ret = static_cast<int>(
		    read(m_fd, &(m_in_buffer->m_buffer[write_index]), read_count));
		DEBUGFMTLOG("success read {} bytes from addr[{}], client fd[{}]", ret,
		         m_peer_addr->toString().c_str(), m_fd);

		if (ret > 0) {
			m_in_buffer->moveWriteIndex(ret);
			if (ret == read_count) {
				continue;
			} else if (ret < read_count) {
				is_read_all = true;
				break;
			}
		} else if (ret == 0) {
			is_close = true;
			break;
		} else if (ret == -1 && errno == EAGAIN) {
			is_read_all = true;
			break;
		}
	}

	if (is_close) {
		INFOFMTLOG("peer closed, peer addr [{}], clientfd []",
		        m_peer_addr->toString().c_str(), m_fd);
		clear();
		return;
	}

	if (!is_read_all) {
		ERRORFMTLOG("not read all data");
	}

	excute();
}

void TcpConnection::excute() {
	if (m_connection_type == TcpConnectionByServer) {
		// 将 RPC 请求执行业务逻辑，获取 RPC 响应, 再把 RPC 响应发送回去
		std::vector<AbstractProtocol::s_ptr> require_messages;
		std::vector<AbstractProtocol::s_ptr> respose_messages;
		m_codec->decode(require_messages, m_in_buffer);

		for (auto& message : require_messages) {
			// 1. 针对每一个请求，调用 rpc 方法，获取响应 message
			// 2. 将响应 message 放入到发送缓冲区，监听可写事件回包
			INFOFMTLOG("success get request[{}] from client[{}]",
			        message->m_msg_id.c_str(), m_peer_addr->toString().c_str());

			std::shared_ptr<TinyPBProtocol> respose_message =
			    std::make_shared<TinyPBProtocol>();

			RpcDispatcher::GetGlobalRpcDispatcher()->dispatch(
			    message, respose_message, this);
			respose_messages.emplace_back(respose_message);
		}

		m_codec->encode(respose_messages, m_out_buffer);
		listenWrite();

	} else {
		// 从 buffer 里 decode 得到 message 对象, 执行其回调
		std::vector<AbstractProtocol::s_ptr> messages;
		m_codec->decode(messages, m_in_buffer);

		for (auto& message : messages) {
			std::string msg_id = message->m_msg_id;
			auto it = m_read_dones.find(msg_id);
			if (it != m_read_dones.end())
				it->second(message);
		}
	}
}

void TcpConnection::onWrite() {
	// 将当前 out_buffer 里面的数据全部发送给 client

	if (m_state != Connected) {
		ERRORFMTLOG("onWrite error, client has already disconneced, addr[{}], "
		         "clientfd[{}]",
		         m_peer_addr->toString().c_str(), m_fd);
		return;
	}

	if (m_connection_type == TcpConnectionByClient) {
		//  1. 将 message encode 得到字节流
		// 2. 将字节流入到 buffer 里面，然后全部发送

		std::vector<AbstractProtocol::s_ptr> messages;

		messages.reserve(m_write_dones.size());
		for (auto& m_write_done : m_write_dones) {
			messages.push_back(m_write_done.first);
		}

		m_codec->encode(messages, m_out_buffer);
	}

	bool is_write_all = false;
	while (true) {
		if (m_out_buffer->readAble() == 0) {
			DEBUGFMTLOG("no data need to send to client [{}]",
			         m_peer_addr->toString().c_str());
			is_write_all = true;
			break;
		}
		int write_size = m_out_buffer->readAble();
		int read_index = m_out_buffer->readIndex();

		int ret = static_cast<int>(
		    write(m_fd, &(m_out_buffer->m_buffer[read_index]), write_size));

		if (ret >= write_size) {
			DEBUGFMTLOG("no data need to send to client [{}]",
			         m_peer_addr->toString().c_str());
			is_write_all = true;
			break;
		}
		if (ret == -1 && errno == EAGAIN) {
			// 发送缓冲区已满，不能再发送了。
			// 这种情况我们等下次 fd 可写的时候再次发送数据即可
			ERRORFMTLOG("write data error, errno==EAGIN and ret == -1");
			break;
		}
	}
	if (is_write_all) {
		m_fd_event->cancel(FdEvent::OUT_EVENT);
		m_eventloop->addEpollEvent(m_fd_event);
	}

	if (m_connection_type == TcpConnectionByClient) {
		for (auto& m_write_done : m_write_dones) {
			m_write_done.second(m_write_done.first);
		}
		m_write_dones.clear();
	}
}

void TcpConnection::clear() {
	// 处理一些关闭连接后的清理动作
	if (m_state == Closed)
		return;

	m_fd_event->cancel(FdEvent::IN_EVENT);  // 关闭读入事件
	m_fd_event->cancel(FdEvent::OUT_EVENT); // 关闭输出事件

	m_eventloop->deleteEpollEvent(m_fd_event); // 将事件从 epoll 中取消注册

	m_state = Closed;
}

void TcpConnection::shutDown() {
	if (m_state == Closed || m_state == NotConnected)
		return;

	// 处于半关闭
	m_state = HalfClosing;

	// 调用 shutdown 关闭读写，意味着服务器不会再对这个 fd 进行读写操作了
	// 发送 FIN 报文， 触发了四次挥手的第一个阶段
	// 当 fd 发生可读事件，但是可读的数据为0，即 对端发送了 FIN
	shutdown(m_fd, SHUT_RDWR);
}

void TcpConnection::listenWrite() {

	m_fd_event->listen(FdEvent::OUT_EVENT,
	                   std::bind(&TcpConnection::onWrite, this));
	m_eventloop->addEpollEvent(m_fd_event);
}

void TcpConnection::listenRead() {

	m_fd_event->listen(FdEvent::IN_EVENT,
	                   std::bind(&TcpConnection::onRead, this));
	m_eventloop->addEpollEvent(m_fd_event);
}

void TcpConnection::pushSendMessage(
    const AbstractProtocol::s_ptr& message,
    const std::function<void(AbstractProtocol::s_ptr)>& done) {
	m_write_dones.emplace_back(message, done);
}

void TcpConnection::pushReadMessage(
    const std::string& msg_id,
    const std::function<void(AbstractProtocol::s_ptr)>& done) {
	m_read_dones.insert(std::make_pair(msg_id, done));
}

MRPC_NAMESPACE_END
