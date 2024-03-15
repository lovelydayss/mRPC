#include "fd_event_group.h"
#include "config.h"
#include "fd_event.h"
#include "utils.h"
#include <cstddef>
#include <memory>
#include <mutex>

MRPC_NAMESPACE_BEGIN

static FdEventGroup::s_ptr s_ptr_FdEventGroup = nullptr;
static std::once_flag singleton_FdEventGroup;

FdEventGroup::FdEventGroup() {
    for (int i = 0; i < Config::GetGlobalConfig()->getFdEventNums(); i++) {
        m_fd_group.push_back(std::make_shared<FdEvent>(i));
    }
}

FdEvent::s_ptr FdEventGroup::getFdEvent(int fd) {

    std::lock_guard<std::mutex> lk(m_mutex);

    if (static_cast<size_t>(fd) < m_fd_group.size())
        return m_fd_group[fd];
    

    int new_size = static_cast<int>(fd * 1.5);

    for (int i = static_cast<int>(m_fd_group.size()); i < new_size; ++i) {
        m_fd_group.push_back(std::make_shared<FdEvent>(i));
    }

    return m_fd_group[fd];
}

const FdEventGroup::s_ptr& FdEventGroup::GetGlobalFdEventGroup() {

    std::call_once(singleton_FdEventGroup, []() {
        s_ptr_FdEventGroup = std::make_shared<FdEventGroup>();
    });

    return s_ptr_FdEventGroup;
}

MRPC_NAMESPACE_END