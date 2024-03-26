#include "wakeup_fd_event.h"
#include "utils.h"
#include <unistd.h>

MRPC_NAMESPACE_BEGIN

void WakeUpFdEvent::wakeup() {

	char buf[8] = {'a'};
	int ret = static_cast<int>(write(m_fd, buf, 8));

	if (ret != 8)
		ERRORFMTLOG("write to wakeup fd less than 8 bytes, fd[{}]", m_fd);

	DEBUGFMTLOG("success read 8 bytes");
}

MRPC_NAMESPACE_END
