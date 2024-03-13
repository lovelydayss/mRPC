#include "sys/timerfd.h"

#include "fd_event.h"
#include "log.h"
#include "timer.h"
#include "timer_event.h"
#include "utils.h"
#include <bits/types/struct_itimerspec.h>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <memory>
#include <mutex>

MRPC_NAMESPACE_BEGIN

Timer::Timer()
    : FdEvent() {

    m_fd = timerfd_create(CLOCK_MONOTONIC,
                          TFD_NONBLOCK | TFD_CLOEXEC); // 创建定时器 fd
    DEBUGLOG("timer fd=%d", m_fd);

    listen(FdEvent::IN_EVENT, std::bind(&Timer::onTimer, this));
}

void Timer::addTimerEvent(const std::shared_ptr<TimerEvent> &event) {
    bool is_rest_timerfd = false;

    {
        std::lock_guard<std::mutex> lk(m_mutex);

        if (m_pending_events.empty()) {     // 定时任务队列为空情况
            is_rest_timerfd = true;
        } else {
            auto event_begin = m_pending_events.begin();
            if (event_begin->second->getArriveTime() > event->getArriveTime())      // 最近定时任务时间比当前待处理任务远情况
                is_rest_timerfd = true;
        }

        m_pending_events.emplace(event->getArriveTime(), event);
    }

    if (is_rest_timerfd) resetArriveTime();
}
void Timer::deleteTimerEvent(const std::shared_ptr<TimerEvent> &event) {

    event->setCancled(true);

    {
        std::lock_guard<std::mutex> lk(m_mutex);

        auto begin = m_pending_events.lower_bound(event->getArriveTime());
        auto end = m_pending_events.upper_bound(event->getArriveTime());

        for (auto &it = begin; it != end; ++it) {
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
    int64_t now = getNowMs();

    std::vector<std::shared_ptr<TimerEvent>> ontime_events;

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

    // 重复触发事件处理
    for (auto &event : ontime_events) {
        if (event->isRepeated()) {
            event->resetArriveTime(); // 调整 arriveTime
            addTimerEvent(event);     // 重新添加到定时器
        }
    }

    // 重设 epoll 触发时间
    resetArriveTime();

    // 执行所有非空定时任务
    for (auto &event : ontime_events) {
        if (event->getCallBack()) event->getCallBack();
    }
}

// 重设 epoll 定时器接口触发时间
void Timer::resetArriveTime() {

    std::unique_lock<std::mutex> ul(m_mutex);

    if (m_pending_events.empty()) return;
    auto event_begin = m_pending_events.begin();

    ul.unlock();

    int64_t now = getNowMs(); // 当前时间
    int64_t interval = 0;     // 时间间隔

    if (event_begin->second->getArriveTime() > now)
        interval = event_begin->second->getArriveTime() - now;
    else
        interval = 100;

    timespec ts{};
    memset(&ts, 0, sizeof(ts));
    ts.tv_sec = interval / 1000;
    ts.tv_nsec = (interval % 1000) * 1000000;

    itimerspec value{};
    memset(&value, 0, sizeof(value));
    value.it_value = ts;

    // 将最近事件绑定到 epoll 监听
    int rt = timerfd_settime(m_fd, 0, &value, nullptr);
    if (rt != 0) {
        ERRORLOG("timerfd_settime error, errno=%d, error=%s", errno,
                 strerror(errno));
    }
    DEBUGLOG("timer reset to %lld", now + interval);
}

MRPC_NAMESPACE_END