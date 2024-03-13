#include "fd_event.h"
#include "utils.h"
#include <sys/epoll.h>

MRPC_NAMESPACE_BEGIN

std::function<void()> FdEvent::handler(TriggerEvent event_type) {
    return event_type == TriggerEvent::IN_EVENT ? m_read_callback
                                                : m_write_callback;
}

void FdEvent::listen(TriggerEvent event_type,
                     const std::function<void()>& callback) {
    if (event_type == TriggerEvent::IN_EVENT) {
        m_listen_events.events |= EPOLLIN;
        m_read_callback = callback;
    } else {
        m_listen_events.events |= EPOLLOUT;
        m_write_callback = callback;
    }

    m_listen_events.data.ptr = this;
}

MRPC_NAMESPACE_END
