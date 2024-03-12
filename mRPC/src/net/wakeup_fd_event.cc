#include "wakeup_fd_event.h"
#include "log.h"
#include "utils.h"
#include <unistd.h>

MRPC_NAMESPACE_BEGIN

void WakeUpFdEvent::wakeup() {

    char buf[8] = {'a'};
    int ret = static_cast<int>(write(m_fd, buf, 8));

    if (ret != 8)
        ERRORLOG("write to wakeup fd less than 8 bytes, fd[%d]", m_fd);

    DEBUGLOG("success read 8 bytes");
}

MRPC_NAMESPACE_END
