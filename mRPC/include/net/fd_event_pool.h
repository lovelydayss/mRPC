#ifndef MRPC_NET_FD_EVENT_POOL_H
#define MRPC_NET_FD_EVENT_POOL_H

#include "config.h"
#include "fd_event.h"
#include <memory>
#include <mutex>
#include <vector>

#include "utils.h"

MRPC_NAMESPACE_BEGIN

class FdEventPool {

  public:
    using s_ptr = std::shared_ptr<FdEventPool>;

  public:
    FdEventPool() = default;
    ~FdEventPool() = default;

  public:
    FdEvent::s_ptr getFdEvent(int fd);
    static const FdEventPool::s_ptr& GetGlobalFdEventPool();

  private:
    int m_size{0};
    std::vector<FdEvent::s_ptr> m_fd_group;
    std::mutex m_mutex;
};

MRPC_NAMESPACE_END

#endif
