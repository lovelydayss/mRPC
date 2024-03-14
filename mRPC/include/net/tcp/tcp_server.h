#ifndef MRPC_NET_TCP_TCP_SERVER_H
#define MRPC_NET_TCP_TCP_SERVER_H

#include "eventloop.h"
#include "fd_event.h"
#include "io_thread_group.h"
#include "net_addr.h"
#include "tcp_acceptor.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

class TcpServer {
  public:
    explicit TcpServer(const std::shared_ptr<NetAddr>& local_addr);
    ~TcpServer() = default;

    void start();

  private:
    void onAccept();

  private:
    std::shared_ptr<TcpAcceptor> m_accepter;
    std::shared_ptr<NetAddr> m_local_addr;

    std::shared_ptr<Eventloop> m_main_event_loop;
    std::shared_ptr<IOThreadGroup> m_io_thread_group;

    std::shared_ptr<FdEvent> m_listen_fd_event;
    int m_client_count{0};
};

MRPC_NAMESPACE_END

#endif