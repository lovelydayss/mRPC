#ifndef MRPC_NET_TCP_TCP_CONNECTION_POOL_H
#define MRPC_NET_TCP_TCP_CONNECTION_POOL_H


#include "tcp_connection.h"
#include "utils.h"
#include <memory>
#include <unordered_map>
#include <list>



MRPC_NAMESPACE_BEGIN


class TcpConnectionPool {

public:
    using s_ptr = std::shared_ptr<TcpConnectionPool>;
    using u_ptr = std::unique_ptr<TcpConnectionPool>;


public:
    static const TcpConnectionPool::s_ptr& GetGlobalTcpConnectionPool();


private:
    std::list<TcpConnection::s_ptr> m_tcp_connections;
    std::unordered_map<TcpConnection::s_ptr, std::list<TcpConnection::s_ptr>::iterator> m_connection_map;

};







MRPC_NAMESPACE_END



#endif
