#include "fd_event.h"
#include "utils.h"
#include <fcntl.h>
#include <sys/epoll.h>

MRPC_NAMESPACE_BEGIN

FdEvent::FdEvent(int fd)
    : m_fd(fd) {
    memset(&m_listen_events, 0, sizeof(m_listen_events));
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

// 取消监听
void FdEvent::cancel(TriggerEvent event_type) {
    m_listen_events.events &=
        event_type == TriggerEvent::IN_EVENT ? (~EPOLLIN) : (~EPOLLOUT);
}

const std::function<void()>& FdEvent::handler(TriggerEvent event_type) {
    return event_type == TriggerEvent::IN_EVENT ? m_read_callback
                                                : m_write_callback;
}

void FdEvent::setNonBlock() const {

    int flag = fcntl(m_fd, F_GETFL, 0);
    if (flag & O_NONBLOCK) //
        return;

    fcntl(m_fd, F_SETFL, flag | O_NONBLOCK);
}

MRPC_NAMESPACE_END
