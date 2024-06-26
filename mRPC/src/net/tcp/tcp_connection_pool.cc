#include "tcp_connection_pool.h"
#include "utils.h"
#include <mutex>

MRPC_NAMESPACE_BEGIN

static TcpConnectionPool::s_ptr s_ptr_connection_pool = nullptr;
static std::once_flag singleton_flag;

const TcpConnectionPool::s_ptr& TcpConnectionPool::GetGlobalTcpConnectionPool() {
    
    std::call_once(singleton_flag, [](){
        
    });

    return s_ptr_connection_pool;
}


MRPC_NAMESPACE_END
