#ifndef MRPC_NET_TCP_TCP_ACCEPTOR_H
#define MRPC_NET_TCP_TCP_ACCEPTOR_H

#include "utils.h"
#include "net_addr.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

class TcpAcceptor {
  public:
    explicit TcpAcceptor(const std::shared_ptr<NetAddr>& local_addr);
    ~TcpAcceptor() = default;

    int accept() const;

  private:
    std::shared_ptr<NetAddr> m_local_addr;      // 服务端监听的地址，addr -> ip:port

    int m_family{-1};                           // 类型
    int m_listenfd{-1};                         // 监听套接字
};

MRPC_NAMESPACE_END

#endif
