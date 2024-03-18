#include "timer.h"
#include "fd_event.h"
#include "log.h"
#include "sys/timerfd.h"
#include "timer_event.h"
#include "utils.h"
#include <bits/types/struct_itimerspec.h>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <mutex>

MRPC_NAMESPACE_BEGIN

Timer::Timer()
    : FdEvent() {

	m_fd = timerfd_create(CLOCK_MONOTONIC,
	                      TFD_NONBLOCK | TFD_CLOEXEC); // 创建定时器 fd
	DEBUGLOG("timer fd=%d", m_fd);

	listen(FdEvent::IN_EVENT, std::bind(&Timer::onTimer, this));
}

void Timer::addTimerEvent(const TimerEvent::s_ptr& event) {
	bool is_rest_timerfd = false;

	{
		std::lock_guard<std::mutex> lk(m_mutex);

		if (m_pending_events.empty()) { // 定时任务队列为空情况
			is_rest_timerfd = true;
		} else {
			auto event_begin = m_pending_events.begin();
			if (event_begin->second->getArriveTime() >
			    event
			        ->getArriveTime()) // 最近定时任务时间比当前待处理任务远情况
				is_rest_timerfd = true;
		}

		m_pending_events.emplace(event->getArriveTime(), event);
	}

	if (is_rest_timerfd)
		resetArriveTime();
}
void Timer::deleteTimerEvent(const TimerEvent::s_ptr& event) {

	event->setCancled(true);

	{
		std::lock_guard<std::mutex> lk(m_mutex);

		auto begin = m_pending_events.lower_bound(event->getArriveTime());
		auto end = m_pending_events.upper_bound(event->getArriveTime());

		for (auto& it = begin; it != end; ++it) {
			if (it->second == event) {
				m_pending_events.erase(it);
				break;
			}
		}

		DEBUGLOG("success delete TimerEvent at arrive time %lld",
		         event->getArriveTime());
	}
}

// 定时器触发时执行
void Timer::onTimer() {

	// 处理缓冲区数据，防止下一次继续触发可读事件
	DEBUGLOG("ontimer");
	char buf[8];
	while (true) {
		if ((read(m_fd, buf, 8) == -1) && errno == EAGAIN) {
			break;
		}
	}

	// 执行定时任务
	int64_t now = getNowNanoseconds();

	std::vector<TimerEvent::s_ptr> ontime_events;

	{
		std::lock_guard<std::mutex> lk(m_mutex);
		auto it = m_pending_events.begin();

		for (; it != m_pending_events.end() && (*it).first <= now; ++it) {
			if (!(*it).second->isCancled()) {
				ontime_events.push_back((*it).second);
			}
		}

		m_pending_events.erase(m_pending_events.begin(), it);
	}

	for (auto& event : ontime_events) {
		if (event->getCallBack()) {
			event->getCallBack()(); // 执行任务
		}

		if (event->isRepeated()) {
			event->resetArriveTime(); // 调整 arriveTime
			addTimerEvent(event);     // 重新添加到定时器
		}
	}

	resetArriveTime(); // 重设定时器触发
}

// 重设 epoll 定时器接口触发时间
void Timer::resetArriveTime() {

	std::unique_lock<std::mutex> ul(m_mutex);

	if (m_pending_events.empty())
		return;
	auto event_begin = m_pending_events.begin();

	ul.unlock();

	int64_t now = getNowNanoseconds(); // 当前时间
	int64_t interval = 0;               // 时间间隔

	if (event_begin->second->getArriveTime() > now)
		interval = event_begin->second->getArriveTime() - now;
	else
		interval = MILLISECONDS(100);

	timespec ts{};
	memset(&ts, 0, sizeof(ts));
	ts.tv_sec = interval / SECONDS(1);
	ts.tv_nsec = interval % SECONDS(1);

	itimerspec value{};
	memset(&value, 0, sizeof(value));
	value.it_value = ts;

	// 将最近事件绑定到 epoll 监听
	int ret = timerfd_settime(m_fd, 0, &value, nullptr);
	if (ret != 0) {
		ERRORLOG("timerfd_settime error, errno=%d, error=%s", errno,
		         strerror(errno));
	}
	DEBUGLOG("timer reset to %lld", now + interval);
}

MRPC_NAMESPACE_END