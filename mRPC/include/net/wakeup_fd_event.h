#ifndef MRPC_NET_WAKEUP_FD_EVENT_H
#define MRPC_NET_WAKEUP_FD_EVENT_H

#include "fd_event.h"
#include "utils.h"

MRPC_NAMESPACE_BEGIN

class WakeUpFdEvent : public FdEvent {
    public:
    using s_ptr = std::shared_ptr<WakeUpFdEvent>;
  public:
    explicit WakeUpFdEvent(int fd)
        : FdEvent(fd) {}
    ~WakeUpFdEvent() = default;

    void wakeup();

  private:
};

MRPC_NAMESPACE_END

#endif
