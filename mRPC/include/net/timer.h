#ifndef MRPC_NET_TIMER_H
#define MRPC_NET_TIMER_H

#include "fd_event.h"
#include "timer_event.h"
#include "utils.h"
#include <functional>
#include <map>
#include <memory>
#include <mutex>

MRPC_NAMESPACE_BEGIN

class Timer : public FdEvent {
  public:
    using s_ptr = std::shared_ptr<Timer>;

  public:
    Timer();
    ~Timer() = default;

    void addTimerEvent(const TimerEvent::s_ptr& event);
    void deleteTimerEvent(const TimerEvent::s_ptr& event);

    void onTimer();

  private:
    void resetArriveTime();

  private:
    std::multimap<int64_t, TimerEvent::s_ptr> m_pending_events;
    std::mutex m_mutex;
};

MRPC_NAMESPACE_END

#endif
