#ifndef MRPC_NET_TCP_TCP_CLIENT_H
#define MRPC_NET_TCP_TCP_CLIENT_H

#include "abstract_protocol.h"
#include "eventloop.h"
#include "fd_event.h"
#include "net_addr.h"
#include "tcp_connection.h"
#include "utils.h"
#include <cstdint>
#include <memory>

MRPC_NAMESPACE_BEGIN

class TcpClient {
public:
	using s_ptr = std::shared_ptr<TcpClient>;

public:
	explicit TcpClient(const NetAddr::s_ptr& peer_addr);

	~TcpClient();

	// 异步的进行 conenct
	// 如果connect 成功，done 会被执行
	void connect(const std::function<void()>& done);

	// 结束连接
	void stop();

	// 异步的发送 message
	// 如果发送 message 成功，会调用 done 函数， 函数的入参就是 message 对象
	void writeMessage(const AbstractProtocol::s_ptr& message,
	                  const std::function<void(AbstractProtocol::s_ptr)>& done);

	// 异步的读取 message
	// 如果读取 message 成功，会调用 done 函数， 函数的入参就是 message 对象
	void readMessage(const AbstractProtocol::s_ptr& message,
	                 const std::function<void(AbstractProtocol::s_ptr)>& done);

	uint32_t getConnectErrorCode() const { return m_connect_error_code; }
	const std::string& getConnectErrorInfo() const {
		return m_connect_error_info;
	}

	const NetAddr::s_ptr& getPeerAddr() { return m_peer_addr; }
	NetAddr::s_ptr getLocalAddr() { return m_local_addr; }

	void addTimerEvent(const TimerEvent::s_ptr& timer_event) {
		m_event_loop->addTimerEvent(timer_event);
	}

private:
	void initLocalAddr();

private:
	NetAddr::s_ptr m_peer_addr;
	NetAddr::s_ptr m_local_addr;
	EventLoop::s_ptr m_event_loop;

	int m_fd{-1};
	FdEvent::s_ptr m_fd_event;

	TcpConnection::s_ptr m_connection;

	uint32_t m_connect_error_code{0};
	std::string m_connect_error_info;
};

MRPC_NAMESPACE_END

#endif