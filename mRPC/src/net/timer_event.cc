#include "timer_event.h"
#include "utils.h"

MRPC_NAMESPACE_BEGIN

TimerEvent::TimerEvent(int64_t interval, bool is_repeated,
                       const std::function<void()>& cb)
    : m_interval(interval)
    , m_is_repeated(is_repeated)
    , m_task(cb) {

	resetArriveTime();
}

void TimerEvent::resetArriveTime() {

	m_arrive_time = getNowNanoseconds() + m_interval;
	DEBUGFMTLOG("success create timer event, will excute at [{}]",
	         m_arrive_time);
}

MRPC_NAMESPACE_END