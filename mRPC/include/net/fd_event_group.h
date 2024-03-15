#ifndef MRPC_NET_FD_EVENT_GROUP_H
#define MRPC_NET_FD_EVENT_GROUP_H

#include "config.h"
#include "fd_event.h"
#include <memory>
#include <mutex>
#include <vector>

#include "utils.h"

MRPC_NAMESPACE_BEGIN

class FdEventGroup {

  public:
    using s_ptr = std::shared_ptr<FdEventGroup>;

  public:
    FdEventGroup();
    ~FdEventGroup() = default;

    FdEvent::s_ptr getFdEvent(int fd);

  public:
    static const FdEventGroup::s_ptr& GetGlobalFdEventGroup();

  private:
    int m_size{0};
    std::vector<FdEvent::s_ptr> m_fd_group;
    std::mutex m_mutex;
};

MRPC_NAMESPACE_END

#endif
