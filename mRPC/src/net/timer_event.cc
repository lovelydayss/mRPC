#include "timer_event.h"
#include "log.h"
#include "utils.h"

MRPC_NAMESPACE_BEGIN

TimerEvent::TimerEvent(int interval, bool is_repeated, std::function<void()> cb)
    : m_interval(interval)
    , m_is_repeated(is_repeated)
    , m_task(std::move(cb)) {

    resetArriveTime();
}

void TimerEvent::resetArriveTime() {

    m_arrive_time = getNowMs() + m_interval;
    DEBUGLOG("success create timer event, will excute at [%lld]",
             m_arrive_time);
}

MRPC_NAMESPACE_END