#ifndef MRPC_NET_FD_EVENT_H
#define MRPC_NET_FD_EVENT_H

#include <cstring>
#include <sys/epoll.h>

#include <functional>

#include "utils.h"

MRPC_NAMESPACE_BEGIN

class FdEvent {

  public:
    enum TriggerEvent {
        IN_EVENT = EPOLLIN,
        OUT_EVENT = EPOLLOUT,
    };

    FdEvent() { memset(&m_listen_events, 0, sizeof(m_listen_events)); }
    explicit FdEvent(int fd)
        : m_fd(fd) {
        memset(&m_listen_events, 0, sizeof(m_listen_events));
    }

    ~FdEvent() = default;

    const std::function<void()>&
        handler(TriggerEvent event_type); // 根据套接字类型返回对应回调函数
    void listen(
        TriggerEvent event_type,
        const std::function<void()>& callback); // 监听时给套接字绑定回调函数

    int getFd() const { return m_fd; }
    epoll_event getEpollEvent() { return m_listen_events; }

  protected:
    int m_fd{-1};
    epoll_event m_listen_events{};

    std::function<void()> m_read_callback;
    std::function<void()> m_write_callback;
};

MRPC_NAMESPACE_END

#endif
