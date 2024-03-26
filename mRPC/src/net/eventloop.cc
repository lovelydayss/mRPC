#include "eventloop.h"
#include "fd_event.h"
#include "timer.h"
#include "timer_event.h"
#include "utils.h"
#include "wakeup_fd_event.h"
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <unistd.h>

MRPC_NAMESPACE_BEGIN

#define ADD_TO_EPOLL()                                                      \
	auto it = m_listen_fds.find(event->getFd());                            \
	int op = EPOLL_CTL_ADD;                                                 \
	if (it != m_listen_fds.end()) {                                         \
		op = EPOLL_CTL_MOD;                                                 \
	}                                                                       \
	epoll_event tmp = event->getEpollEvent();                               \
	INFOFMTLOG("epoll_event.events = {}", (int)tmp.events);                    \
	int ret = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp);              \
	if (ret == -1) {                                                        \
		ERRORFMTLOG("failed epoll_ctl when add fd, errno={}, error={}", errno, \
		         strerror(errno));                                          \
	}                                                                       \
	m_listen_fds.insert(event->getFd());                                    \
	DEBUGFMTLOG("add event success, fd[{}]", event->getFd());

#define DELETE_TO_EPOLL()                                                      \
	auto it = m_listen_fds.find(event->getFd());                               \
	if (it == m_listen_fds.end()) {                                            \
		return;                                                                \
	}                                                                          \
	int op = EPOLL_CTL_DEL;                                                    \
	epoll_event tmp = event->getEpollEvent();                                  \
	int ret = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp);                 \
	if (ret == -1) {                                                           \
		ERRORFMTLOG("failed epoll_ctl when delete fd, errno={}, error={}", errno, \
		         strerror(errno));                                             \
	}                                                                          \
	DEBUGFMTLOG("delete event success, fd[{}]", event->getFd());                  \
	m_listen_fds.erase(event->getFd());

static thread_local EventLoop::s_ptr t_current_EventLoop = nullptr;
static thread_local std::once_flag t_singleton_EventLoop;
static int g_epoll_max_timeout = 10000;
static int g_epoll_max_events = 10;

EventLoop::~EventLoop() { close(m_epoll_fd); }

EventLoop::s_ptr EventLoop::GetThreadLocalEventLoop() {

	std::call_once(t_singleton_EventLoop, [&]() {
		t_current_EventLoop = std::make_shared<EventLoop>();
		t_current_EventLoop->m_thread_id = getThreadId();
		int m_epoll_fd = epoll_create(10);

		if (m_epoll_fd == -1) {
			ERRORFMTLOG(
			    "failed to create event loop, epoll_create error, error "
			    "info[{}]",
			    errno);
			exit(0);
		}

		t_current_EventLoop->m_epoll_fd = m_epoll_fd;

		t_current_EventLoop->initWakeUpFdEevent();
		t_current_EventLoop->initTimer();

		INFOFMTLOG("success create event loop in thread {}",
		           t_current_EventLoop->m_thread_id);
	});

	return t_current_EventLoop;
}

void EventLoop::loop() {
	m_is_looping = true;

	while (!m_stop_flag) {

		std::queue<std::function<void()>> tmp_tasks;

		{
			std::lock_guard<std::mutex> lk(m_mutex);
			m_pending_tasks.swap(tmp_tasks);
		}

		while (!tmp_tasks.empty()) {
			std::function<void()> cb = tmp_tasks.front();
			tmp_tasks.pop();

			if (cb)
				cb();
		}

		int timeout = g_epoll_max_timeout;
		epoll_event result_events[g_epoll_max_events];
		int ret =
		    epoll_wait(m_epoll_fd, result_events, g_epoll_max_events, timeout);
		DEBUGFMTLOG("now end epoll_wait, ret = {}", ret);

		if (ret < 0) {
			ERRORFMTLOG("epoll_wait error, errno = {}", errno);
		} else {
			for (int i = 0; i < ret; i++) {
				epoll_event trigger_event = result_events[i];
				//  FdEvent* fd_event =
				//  static_cast<FdEvent*>(trigger_event.data.ptr);
				FdEvent::s_ptr fd_event = std::make_shared<FdEvent>(
				    *(static_cast<FdEvent*>(trigger_event.data.ptr)));

				if (fd_event == nullptr) {
					ERRORFMTLOG("fd_event = NULL, continue");
					continue;
				}

				if (trigger_event.events & EPOLLIN) {

					DEBUGFMTLOG("fd {} trigger EPOLLIN event",
					            fd_event->getFd());
					addTask(fd_event->handler(FdEvent::IN_EVENT));
				}

				if (trigger_event.events & EPOLLOUT) {
					DEBUGFMTLOG("fd {} trigger EPOLLOUT event",
					            fd_event->getFd());
					addTask(fd_event->handler(FdEvent::OUT_EVENT));
				}
				// EPOLLHUP EPOLLERR
				// if (trigger_event.events & EPOLLERR) {
				else {
					DEBUGFMTLOG("fd {} trigger EPOLLERROR event",
					            fd_event->getFd());

					// 删除出错的套接字
					deleteEpollEvent(fd_event);
					if (fd_event->handler(FdEvent::ERROR_EVENT) != nullptr) {
						DEBUGFMTLOG("fd {} add error callback", fd_event->getFd());
						addTask(fd_event->handler(FdEvent::OUT_EVENT));
					}
				}
			}
		}
	}
}
void EventLoop::stop() {

	INFOFMTLOG("EVENT LOOP STOP!");
	m_stop_flag = true;
}
void EventLoop::wakeup() {

	INFOFMTLOG("WAKE UP!");
	m_wakeup_fd_event->wakeup();
}

void EventLoop::addEpollEvent(const FdEvent::s_ptr& event) {
	if (isInLoopThread()) {
		ADD_TO_EPOLL();
	} else {
		auto cb = [this, event]() { ADD_TO_EPOLL(); };
		addTask(cb, true);
	}
}
void EventLoop::deleteEpollEvent(const FdEvent::s_ptr& event) {
	if (isInLoopThread()) {
		DELETE_TO_EPOLL();
	} else {
		auto cb = [this, event]() { DELETE_TO_EPOLL(); };
		addTask(cb, true);
	}
}

void EventLoop::addTimerEvent(const TimerEvent::s_ptr& event) {
	m_timer->addTimerEvent(event);
}

void EventLoop::deleteTimerEvent(const TimerEvent::s_ptr& event) {
	m_timer->deleteTimerEvent(event);
}

bool EventLoop::isInLoopThread() const { return getThreadId() == m_thread_id; }
bool EventLoop::isLooping() const { return m_is_looping; }

void EventLoop::addTask(const std::function<void()>& cb, bool is_wake_up) {

	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_pending_tasks.push(cb);
	}

	if (is_wake_up)
		wakeup();
}

void EventLoop::dealWakeup() {}

void EventLoop::initTimer() {
	m_timer = std::make_shared<Timer>();
	addEpollEvent(m_timer);
}
void EventLoop::initWakeUpFdEevent() {
	m_wakeup_fd = eventfd(0, EFD_NONBLOCK);

	if (m_wakeup_fd < 0) {
		ERRORFMTLOG("failed to create event loop, eventfd create error, error "
		         "info[{}]",
		         errno);
		exit(0);
	}

	INFOFMTLOG("wakeup fd = {}", m_wakeup_fd);

	m_wakeup_fd_event = std::make_shared<WakeUpFdEvent>(m_wakeup_fd);
	m_wakeup_fd_event->listen(FdEvent::IN_EVENT, [&]() {
		char buf[8];
		while (read(m_wakeup_fd, buf, 8) != -1 && errno != EAGAIN) {
		}

		DEBUGFMTLOG("read full bytes from wakeup fd[{}]", m_wakeup_fd);
	});

	addEpollEvent(m_wakeup_fd_event);
}

MRPC_NAMESPACE_END