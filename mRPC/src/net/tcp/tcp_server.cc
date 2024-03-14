#include "tcp_server.h"
#include "config.h"
#include "eventloop.h"
#include "fd_event.h"
#include "io_thread_group.h"
#include "tcp_acceptor.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

TcpServer::TcpServer(const std::shared_ptr<NetAddr>& local_addr)
    : m_local_addr(local_addr) {

    m_accepter = std::make_shared<TcpAcceptor>(m_local_addr);
    m_main_event_loop = Eventloop::GetThreadLocalEventloop();
    m_io_thread_group = std::make_shared<IOThreadGroup>(
        Config::GetGlobalConfig()->getIOThreadNums());

    m_listen_fd_event = std::make_shared<FdEvent>(m_accepter->getListenFd());
    m_listen_fd_event->listen(FdEvent::IN_EVENT,
                              std::bind(&TcpServer::onAccept, this));
    m_main_event_loop->addEpollEvent(m_listen_fd_event);
}

void TcpServer::start() {
    m_io_thread_group->start();
    m_main_event_loop->loop();
}

void TcpServer::onAccept() {
    int client_fd = m_accepter->accept();
    // FdEvent client_fd_event(client_fd);
    m_client_count++;

    // TODO: 把 cleintfd 添加到任意 IO 线程里面
    // m_io_thread_group->getIOThread()->getEventLoop()->addEpollEvent(client_fd_event);

    INFOLOG("TcpServer succ get client, fd=%d", client_fd);
}

MRPC_NAMESPACE_END