#include "tcp_connection.h"
#include "config.h"
#include "fd_event_pool.h"
#include "log.h"
#include "net_addr.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

TcpConnection::TcpConnection(const EventLoop::s_ptr& event_loop, int fd,
                             const NetAddr::s_ptr& peer_addr)
    : m_event_loop(event_loop)
    , m_peer_addr(peer_addr)
    , m_fd(fd) {

	m_in_buffer = std::make_shared<TcpBuffer>(
	    Config::GetGlobalConfig()->getConnectionBufferSize());
	m_out_buffer = std::make_shared<TcpBuffer>(
	    Config::GetGlobalConfig()->getConnectionBufferSize());

	m_fd_event = FdEventPool::GetGlobalFdEventPool()->getFdEvent(fd);
	m_fd_event->setNonBlock();
	m_fd_event->listen(FdEvent::IN_EVENT,
	                   std::bind(&TcpConnection::onRead, this));
	m_event_loop->addEpollEvent(m_fd_event);
}

void TcpConnection::onRead() {
	// 1. 从 socket 缓冲区，调用 系统的 read 函数读取字节 in_buffer 里面

	if (m_state != Connected) {
		ERRORLOG("onRead error, client has already disconneced, addr[%s], "
		         "clientfd[%d]",
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
		DEBUGLOG("success read %d bytes from addr[%s], client fd[%d]", ret,
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
		// TODO:
		INFOLOG("peer closed, peer addr [%s], clientfd [%d]",
		        m_peer_addr->toString().c_str(), m_fd);
		clear();
		return;
	}

	if (!is_read_all) {
		ERRORLOG("not read all data");
	}

	// TODO: 简单的 echo, 后面补充 RPC 协议解析
	excute();
}

void TcpConnection::excute() {
	// 将 RPC 请求执行业务逻辑，获取 RPC 响应, 再把 RPC 响应发送回去
	std::vector<char> tmp;
	int size = m_in_buffer->readAble();
	tmp.resize(size);
	m_in_buffer->readFromBuffer(tmp, size);

	std::string msg;
	for (size_t i = 0; i < tmp.size(); ++i) {
		msg += tmp[i];
	}

	INFOLOG("success get request[%s] from client[%s]", msg.c_str(),
	        m_peer_addr->toString().c_str());

	m_out_buffer->writeToBuffer(msg.c_str(), static_cast<int>(msg.length()));

	m_fd_event->listen(FdEvent::OUT_EVENT,
	                   std::bind(&TcpConnection::onWrite, this));
	m_event_loop->addEpollEvent(m_fd_event);
}

void TcpConnection::onWrite() {
	// 将当前 out_buffer 里面的数据全部发送给 client

	if (m_state != Connected) {
		ERRORLOG("onWrite error, client has already disconneced, addr[%s], "
		         "clientfd[%d]",
		         m_peer_addr->toString().c_str(), m_fd);
		return;
	}

	bool is_write_all = false;
	while (true) {
		if (m_out_buffer->readAble() == 0) {
			DEBUGLOG("no data need to send to client [%s]",
			         m_peer_addr->toString().c_str());
			is_write_all = true;
			break;
		}
		int write_size = m_out_buffer->readAble();
		int read_index = m_out_buffer->readIndex();

		int ret = static_cast<int>(
		    write(m_fd, &(m_out_buffer->m_buffer[read_index]), write_size));

		if (ret >= write_size) {
			DEBUGLOG("no data need to send to client [%s]",
			         m_peer_addr->toString().c_str());
			is_write_all = true;
			break;
		}
		if (ret == -1 && errno == EAGAIN) {
			// 发送缓冲区已满，不能再发送了。
			// 这种情况我们等下次 fd 可写的时候再次发送数据即可
			ERRORLOG("write data error, errno==EAGIN and ret == -1");
			break;
		}
	}
	if (is_write_all) {
		m_fd_event->cancel(FdEvent::OUT_EVENT);
		m_event_loop->addEpollEvent(m_fd_event);
	}
}

void TcpConnection::clear() {
	// 处理一些关闭连接后的清理动作
	if (m_state == Closed)
		return;

	m_fd_event->cancel(FdEvent::IN_EVENT);  // 关闭读入事件
	m_fd_event->cancel(FdEvent::OUT_EVENT); // 关闭输出事件

	m_event_loop->deleteEpollEvent(m_fd_event); // 将事件从 epoll 中取消注册

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

MRPC_NAMESPACE_END
