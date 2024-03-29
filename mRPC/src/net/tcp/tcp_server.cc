#include "tcp_server.h"
#include "config.h"
#include "eventloop.h"
#include "fd_event.h"
#include "io_thread.h"
#include "io_thread_pool.h"
#include "net_addr.h"
#include "tcp_acceptor.h"
#include "tcp_connection.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

TcpServer::TcpServer(const NetAddr::s_ptr& local_addr)
    : m_local_addr(local_addr) {

	m_accepter = std::make_shared<TcpAcceptor>(m_local_addr);
	m_main_event_loop = EventLoop::GetThreadLocalEventLoop();
	m_io_thread_pool = std::make_shared<IOThreadPool>(
	    Config::GetGlobalConfig()->getIOThreadNums());

	m_listen_fd_event = std::make_shared<FdEvent>(m_accepter->getListenFd());
	m_listen_fd_event->listen(FdEvent::IN_EVENT,
	                          std::bind(&TcpServer::onAccept, this));
	m_main_event_loop->addEpollEvent(m_listen_fd_event);
}

void TcpServer::start() {
	m_io_thread_pool->start();
	m_main_event_loop->loop();
}

void TcpServer::onAccept() {
	auto re = m_accepter->accept();
	int client_fd = re.first;
	NetAddr::s_ptr peer_addr = re.second;
	m_client_count++;

	// 把 cleintfd(connection) 添加到任意 IO 线程里面
	IOThread::s_ptr io_thread = m_io_thread_pool->getIOThread();
	TcpConnection::s_ptr connection = std::make_shared<TcpConnection>(
	    io_thread->getEventLoop(), client_fd, peer_addr, m_local_addr);
	connection->setState(Connected);

	m_connections.insert(connection);
	INFOFMTLOG("TcpServer succ get client connection, fd={}", client_fd);
}

MRPC_NAMESPACE_END