#ifndef MRPC_NET_TCP_TCP_CONNECTION_H
#define MRPC_NET_TCP_TCP_CONNECTION_H

#include "eventloop.h"
#include "fd_event.h"
#include "log.h"
#include "net_addr.h"
#include "tcp_buffer.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

enum TcpState {
	NotConnected = 1, // 无连接
	Connected = 2,    // 连接
	HalfClosing = 3,  // 半连接
	Closed = 4,       // 关闭
};

enum TcpConnectionType {
	TcpConnectionByServer = 1, // 作为服务端使用，代表跟对端客户端的连接
	TcpConnectionByClient = 2, // 作为客户端使用，代表跟对端服务端的连接
};

class TcpConnection {
public:
	using s_ptr = std::shared_ptr<TcpConnection>;

public:
	TcpConnection(const EventLoop::s_ptr& event_loop, int fd,
	              const NetAddr::s_ptr& peer_addr);
	~TcpConnection() { DEBUGLOG("~TcpConnection"); }

	void onRead();  // 读入数据
	void excute();  // 处理逻辑
	void onWrite(); // 回发数据

	TcpState getState() const { return m_state; }
	void setState(const TcpState state) { m_state = state; }

	TcpConnectionType getConnectionType() const { return m_connection_type; }
	void setConnectionType(const TcpConnectionType Connection_type) {
		m_connection_type = Connection_type;
	}

	void clear();    // 关闭后进行清理工作
	void shutDown(); // 服务器主动关闭连接

private:
	EventLoop::s_ptr m_event_loop; // 持有该连接的 IO 线程
	NetAddr::s_ptr m_local_addr;
	NetAddr::s_ptr m_peer_addr;

	TcpBuffer::s_ptr m_in_buffer;  // 接收缓冲区
	TcpBuffer::s_ptr m_out_buffer; // 发送缓冲区

	FdEvent::s_ptr m_fd_event;
	int m_fd{0};

	TcpState m_state{NotConnected};
	TcpConnectionType m_connection_type{TcpConnectionByServer};
};

MRPC_NAMESPACE_END

#endif