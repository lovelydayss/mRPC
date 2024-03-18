#ifndef MRPC_NET_TCP_TCP_SERVER_H
#define MRPC_NET_TCP_TCP_SERVER_H

#include "eventloop.h"
#include "fd_event.h"
#include "io_thread_group.h"
#include "net_addr.h"
#include "tcp_acceptor.h"
#include "tcp_connection.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

class TcpServer {
public:
	using s_ptr = std::shared_ptr<TcpServer>;

public:
	explicit TcpServer(const NetAddr::s_ptr& local_addr);
	~TcpServer() = default;

	void start();

private:
	void onAccept();

private:
	TcpAcceptor::s_ptr m_accepter;
	NetAddr::s_ptr m_local_addr;

	EventLoop::s_ptr m_main_event_loop;
	IOThreadGroup::s_ptr m_io_thread_group;

	FdEvent::s_ptr m_listen_fd_event;
	int m_client_count{0};

	std::set<TcpConnection::s_ptr> m_clients;
};

MRPC_NAMESPACE_END

#endif