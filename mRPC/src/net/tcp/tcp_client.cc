#include "tcp_client.h"
#include "error_code.h"
#include "eventloop.h"
#include "fd_event.h"
#include "fd_event_pool.h"
#include "net_addr.h"
#include "tcp_connection.h"
#include "utils.h"
#include <cerrno>
#include <memory>

MRPC_NAMESPACE_BEGIN

#define SUCCESSFULLY_CONNECT_CALLBACK()                                \
	DEBUGLOG("connect [%s] success", m_peer_addr->toString().c_str()); \
	initLocalAddr();                                                   \
	m_connection->setState(Connected);                                 \
	m_event_loop->deleteEpollEvent(m_fd_event);                        \
                                                                       \
	if (done)                                                          \
		done();

#define FAILED_CONNECT_CALLBACK(connect_error_code)                         \
	ERRORLOG("connect errror, errno=%d, error=%s", errno, strerror(errno)); \
	m_connect_error_code = connect_error_code;                              \
	m_connect_error_info =                                                  \
	    "connect error, sys error = " + std::string(strerror(errno));       \
                                                                            \
	m_event_loop->deleteEpollEvent(m_fd_event); // 取消连接事件监听

TcpClient::TcpClient(const NetAddr::s_ptr& peer_addr,
                     const NetAddr::s_ptr& local_addr /*= nullptr*/) /*NOLINT*/
    : m_peer_addr(peer_addr)
    , m_local_addr(local_addr) {

	// 创建套接字
	m_fd = socket(peer_addr->getFamily(), SOCK_STREAM, 0);

	if (m_fd < 0) {
		ERRORLOG("TcpClient::TcpClient() error, failed to create fd");
		return;
	}

	// 获取本地 Fdevent ( 套接字(后续客户端事件使用此套接字注册到 epoll ))
	m_fd_event = FdEventPool::GetGlobalFdEventPool()->getFdEvent(m_fd);

	// 设置默认为非阻塞
	m_fd_event->setNonBlock();

	// 获取Eventloop
	m_event_loop = EventLoop::GetThreadLocalEventLoop();

	// 创建连接
	m_connection = std::make_shared<TcpConnection>(
	    m_event_loop, m_fd, peer_addr, local_addr, TcpConnectionByClient);
}

TcpClient::~TcpClient() {
	DEBUGLOG("TcpClient::~TcpClient()");
	if (m_fd > 0) {
		close(m_fd);
	}
}

void TcpClient::asyncConnect(const std::function<void()>& done) {
	int ret =
	    connect(m_fd, m_peer_addr->getSockAddr(), m_peer_addr->getSockLen());

	if (ret == 0) {

		SUCCESSFULLY_CONNECT_CALLBACK();

	} else if (ret == -1 && errno == EINPROGRESS) {

		// 设置 epoll 监听可写事件，根据错误码进一步判断
		m_fd_event->listen(FdEvent::OUT_EVENT, [this, done]() {
			int ret = connect(m_fd, m_peer_addr->getSockAddr(),
			                  m_peer_addr->getSockLen());

			if ((ret < 0 && errno == EISCONN) || (ret == 0)) { // 成功连接

				SUCCESSFULLY_CONNECT_CALLBACK();

			} else if (errno == ECONNREFUSED) { // 访问拒绝

				FAILED_CONNECT_CALLBACK(ERROR_PEER_CLOSED);

			} else { // 连接失败，设置错误信息

				FAILED_CONNECT_CALLBACK(ERROR_FAILED_CONNECT);
			}
		});

		// 添加到监听
		m_event_loop->addEpollEvent(m_fd_event);

		// 启动 EventLoop
		if (!m_event_loop->isLooping())
			m_event_loop->loop();

	} else {

		FAILED_CONNECT_CALLBACK(ERROR_FAILED_CONNECT);
	}
}

void TcpClient::stop() {
	if (m_event_loop->isLooping()) {
		m_event_loop->stop();
	}
}

// 异步的发送 message
// 如果发送 message 成功，会调用 done 函数， 函数的入参就是 message 对象
void TcpClient::writeMessage(
    const AbstractProtocol::s_ptr& message,
    const std::function<void(AbstractProtocol::s_ptr)>& done) {
	m_connection->pushSendMessage(message, done);
	m_connection->listenWrite();
}

// 异步的读取 message
// 如果读取 message 成功，会调用 done 函数， 函数的入参就是 message 对象
void TcpClient::readMessage(
    const AbstractProtocol::s_ptr& message,
    const std::function<void(AbstractProtocol::s_ptr)>& done) {
	// 1. 监听可读事件
	// 2. 从 buffer 里 decode 得到 message 对象, 判断是否 msg_id
	// 相等，相等则读成功，执行其回调
	m_connection->pushReadMessage(message->m_msg_id, done);
	m_connection->listenRead();
}

void TcpClient::initLocalAddr() {

	sockaddr_in local_addr{};
	socklen_t len = sizeof(local_addr);

	int ret = getsockname(m_fd, reinterpret_cast<sockaddr*>(&local_addr), &len);
	if (ret != 0) {
		ERRORLOG("initLocalAddr error, getsockname error. errno=%d, error=%s",
		         errno, strerror(errno));
		return;
	}

	m_local_addr = std::make_shared<IPNetAddr>(local_addr);
}

MRPC_NAMESPACE_END
