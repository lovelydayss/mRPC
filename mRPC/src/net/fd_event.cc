#include "fd_event.h"
#include "utils.h"
#include <cstdint>
#include <fcntl.h>
#include <sys/epoll.h>

MRPC_NAMESPACE_BEGIN

FdEvent::FdEvent(int fd)
    : m_fd(fd) {
	memset(&m_listen_events, 0, sizeof(m_listen_events));
}

void FdEvent::listen(TriggerEvent event_type,
                     const std::function<void()>& callback, const std::function<void()>& error_callback /*= nullptr*/) {

	if (event_type == TriggerEvent::IN_EVENT) {
		m_listen_events.events |= EPOLLIN;
		m_read_callback = callback;
	} else {
		m_listen_events.events |= EPOLLOUT;
		m_write_callback = callback;
	}

	if (m_error_callback == nullptr) {
		m_error_callback = error_callback;
	} else {
		m_error_callback = nullptr;
	}

	m_listen_events.data.ptr = this;
}

void FdEvent::cancel(TriggerEvent event_type) {
	m_listen_events.events &=
	    event_type == TriggerEvent::IN_EVENT ? (~EPOLLIN) : (~EPOLLOUT);
}

const std::function<void()>& FdEvent::handler(TriggerEvent event_type) {

	switch (event_type) {
	case TriggerEvent::IN_EVENT:
		return m_read_callback;
	case TriggerEvent::OUT_EVENT:
		return m_write_callback;
	case TriggerEvent::ERROR_EVENT:
		return m_error_callback;
	default:
		DEBUGFMTLOG("wrong fd event type!");
	}
}

void FdEvent::setNonBlock() const {

	uint32_t flag = fcntl(m_fd, F_GETFL, 0);

	if (flag & O_NONBLOCK)
		return;

	fcntl(m_fd, F_SETFL, flag | O_NONBLOCK);
}

void FdEvent::setErrorCallback(const std::function<void()>& callback) {
	m_error_callback = callback;
}

MRPC_NAMESPACE_END
