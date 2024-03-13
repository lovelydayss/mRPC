#ifndef MRPC_NET_TIMER_EVENT_H
#define MRPC_NET_TIMER_EVENT_H

#include "fd_event.h"
#include "utils.h"
#include <cstdint>
#include <functional>

MRPC_NAMESPACE_BEGIN

// 封装一个定时器事件
// 包含到达时间，时间间隔，是否重复，是否取消以及事件触发回调函数等
class TimerEvent {

  public:
    TimerEvent(int interval, bool is_repeated, std::function<void()> cb);

    int64_t getArriveTime() const { return m_arrive_time; }

    void setCancled(bool value) { m_is_cancled = value; }

    bool isCancled() const { return m_is_cancled; }

    bool isRepeated() const { return m_is_repeated; }

    std::function<void()> getCallBack() { return m_task; }

    void resetArriveTime();

  private:
    int64_t m_arrive_time{0};
    int64_t m_interval{0};
    bool m_is_repeated{false};
    bool m_is_cancled{false};

    std::function<void()> m_task;

};

MRPC_NAMESPACE_END

#endif
