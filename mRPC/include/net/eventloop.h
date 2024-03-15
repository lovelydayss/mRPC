#ifndef MRPC_NET_EventLoop_H
#define MRPC_NET_EventLoop_H

#include "fd_event.h"
#include "timer.h"
#include "timer_event.h"
#include "utils.h"
#include "wakeup_fd_event.h"

#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <thread>

MRPC_NAMESPACE_BEGIN

class EventLoop {
  public:
    using s_ptr = std::shared_ptr<EventLoop>;

  public:
    EventLoop() = default;
    ~EventLoop();

    void loop();
    void stop();
    void wakeup();

    void addEpollEvent(const FdEvent::s_ptr& event);
    void deleteEpollEvent(const FdEvent::s_ptr& event);

    void addTimerEvent(const TimerEvent::s_ptr& event);

    bool isInLoopThread() const;
    bool isLooping() const;

    void addTask(const std::function<void()>& cb, bool is_wake_up = false);

    static EventLoop::s_ptr GetThreadLocalEventLoop();


  private:
    void dealWakeup();

    void initTimer();
    void initWakeUpFdEevent();

  private:
    pid_t m_thread_id{0};
    std::mutex m_mutex;

    int m_epoll_fd{0};
    int m_wakeup_fd{0};
    WakeUpFdEvent::s_ptr m_wakeup_fd_event{nullptr};
    Timer::s_ptr m_timer{nullptr};

    std::set<int> m_listen_fds;
    std::queue<std::function<void()>> m_pending_tasks;

    bool m_stop_flag{false};
    bool m_is_looping{false};
};

MRPC_NAMESPACE_END

#endif