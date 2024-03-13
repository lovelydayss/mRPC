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
    Timer();
    ~Timer() = default;

    void addTimerEvent(const std::shared_ptr<TimerEvent>& event);
    void deleteTimerEvent(const std::shared_ptr<TimerEvent>& event);

    void onTimer();

  private:
    void resetArriveTime();

  private:
    std::multimap<int64_t, std::shared_ptr<TimerEvent>> m_pending_events;
    std::mutex m_mutex;
};

MRPC_NAMESPACE_END

#endif
