#ifndef MRPC_NET_EVENTLOOP_H
#define MRPC_NET_EVENTLOOP_H

#include "fd_event.h"
#include "utils.h"
#include "wakeup_fd_event.h"

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <thread>

MRPC_NAMESPACE_BEGIN

class Eventloop {
  public:
    Eventloop();

    ~Eventloop();

    void loop();
    void stop();
    void wakeup();

    void addEpollEvent(FdEvent *event);
    void deleteEpollEvent(FdEvent *event);

    bool isInLoopThread() const;
    void addTask(const std::function<void()>& cb, bool is_wake_up = false);

    static std::shared_ptr<Eventloop> GetThreadLocalEventloop();
  private:
    void dealWakeup();
    void initWakeUpFdEevent();

  private:
    pid_t m_thread_id{0};
    std::mutex m_mutex;

    int m_epoll_fd{0};
    int m_wakeup_fd{0};
    std::unique_ptr<WakeUpFdEvent> m_wakeup_fd_event{nullptr};

    bool m_stop_flag{false};
    std::set<int> m_listen_fds;
    std::queue<std::function<void()>> m_pending_tasks;
};

MRPC_NAMESPACE_END

#endif