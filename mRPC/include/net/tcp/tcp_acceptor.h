#ifndef MRPC_NET_TCP_TCP_ACCEPTOR_H
#define MRPC_NET_TCP_TCP_ACCEPTOR_H

#include "net_addr.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

class TcpAcceptor {
  public:
    using s_ptr = std::shared_ptr<TcpAcceptor>;

  public:
    explicit TcpAcceptor(const NetAddr::s_ptr& local_addr);
    ~TcpAcceptor() = default;

    std::pair<int, NetAddr::s_ptr> accept() const;

    int getListenFd() const { return m_listenfd; }

  private:
    NetAddr::s_ptr m_local_addr; // 服务端监听的地址，addr -> ip:port

    int m_family{-1};   // 类型
    int m_listenfd{-1}; // 监听套接字
};

MRPC_NAMESPACE_END

#endif
