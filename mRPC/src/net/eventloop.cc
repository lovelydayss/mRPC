#include "eventloop.h"
#include "fd_event.h"
#include "log.h"
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

#define ADD_TO_EPOLL()                                                         \
    auto it = m_listen_fds.find(event->getFd());                               \
    int op = EPOLL_CTL_ADD;                                                    \
    if (it != m_listen_fds.end()) {                                            \
        op = EPOLL_CTL_MOD;                                                    \
    }                                                                          \
    epoll_event tmp = event->getEpollEvent();                                  \
    INFOLOG("epoll_event.events = %d", (int) tmp.events);                      \
    int rt = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp);                  \
    if (rt == -1) {                                                            \
        ERRORLOG("failed epoll_ctl when add fd, errno=%d, error=%s", errno,    \
                 strerror(errno));                                             \
    }                                                                          \
    m_listen_fds.insert(event->getFd());                                       \
    DEBUGLOG("add event success, fd[%d]", event->getFd())

#define DELETE_TO_EPOLL()                                                      \
    auto it = m_listen_fds.find(event->getFd());                               \
    if (it == m_listen_fds.end()) {                                            \
        return;                                                                \
    }                                                                          \
    int op = EPOLL_CTL_DEL;                                                    \
    epoll_event tmp = event->getEpollEvent();                                  \
    int rt = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp);                  \
    if (rt == -1) {                                                            \
        ERRORLOG("failed epoll_ctl when add fd, errno=%d, error=%s", errno,    \
                 strerror(errno));                                             \
    }                                                                          \
    DEBUGLOG("delete event success, fd[%d]", event->getFd());

static thread_local std::shared_ptr<Eventloop> t_current_eventloop = nullptr;
static thread_local std::once_flag t_singleton_eventloop;
static int g_epoll_max_timeout = 10000;
static int g_epoll_max_events = 10;

Eventloop::Eventloop() {}

Eventloop::~Eventloop() { close(m_epoll_fd); }

std::shared_ptr<Eventloop> Eventloop::GetThreadLocalEventloop() {

    std::call_once(t_singleton_eventloop, [&]() {
        t_current_eventloop = std::make_shared<Eventloop>();
        t_current_eventloop->m_thread_id = getThreadId();
        int m_epoll_fd = epoll_create(10);

        if (m_epoll_fd == -1) {
            ERRORLOG("failed to create event loop, epoll_create error, error "
                     "info[%d]",
                     errno);
            exit(0);
        }

        t_current_eventloop->m_epoll_fd = m_epoll_fd;

        t_current_eventloop->initWakeUpFdEevent();
        INFOLOG("success create event loop in thread %d", t_current_eventloop->m_thread_id);
    });

    return t_current_eventloop;
}

void Eventloop::loop() {
    while (!m_stop_flag) {

        std::queue<std::function<void()>> tmp_tasks;

        {
            std::lock_guard<std::mutex> lk(m_mutex);
            m_pending_tasks.swap(tmp_tasks);
        }

        while (!tmp_tasks.empty()) {
            std::function<void()> cb = tmp_tasks.front();
            tmp_tasks.pop();

            if (cb) {
                cb();
            }
        }

        int timeout = g_epoll_max_timeout;
        epoll_event result_events[g_epoll_max_events];
        int ret =
            epoll_wait(m_epoll_fd, result_events, g_epoll_max_events, timeout);
        DEBUGLOG("now end epoll_wait, rt = %d", ret);

        if (ret < 0) {
            ERRORLOG("epoll_wait error, errno=", errno);
        } else {
            for (int i = 0; i < ret; i++) {
                epoll_event trigger_event = result_events[i];
                //  FdEvent* fd_event =
                //  static_cast<FdEvent*>(trigger_event.data.ptr);
                std::unique_ptr<FdEvent> fd_event =
                    std::unique_ptr<FdEvent>(new FdEvent(
                        *(static_cast<FdEvent *>(trigger_event.data.ptr))));

                if (fd_event == nullptr) {
                    ERRORLOG("fd_event = NULL, continue");
                    continue;
                }

                if (trigger_event.events & EPOLLIN) {

                    DEBUGLOG("fd %d trigger EPOLLIN event", fd_event->getFd())
                    addTask(fd_event->handler(FdEvent::IN_EVENT));
                }

                if (trigger_event.events & EPOLLOUT) {
                    DEBUGLOG("fd %d trigger EPOLLOUT event", fd_event->getFd())
                    addTask(fd_event->handler(FdEvent::OUT_EVENT));
                }
            }
        }
    }
}
void Eventloop::stop() {

    INFOLOG("EVENT LOOP STOP!");
    m_stop_flag = true;
}
void Eventloop::wakeup() {

    INFOLOG("WAKE UP!");
    m_wakeup_fd_event->wakeup();
}

void Eventloop::addEpollEvent(FdEvent *event) {
    if (isInLoopThread()) {
        ADD_TO_EPOLL();
    } else {
        auto cb = [this, event]() { ADD_TO_EPOLL(); };

        addTask(cb, true);
    }
}
void Eventloop::deleteEpollEvent(FdEvent *event) {
    if (isInLoopThread()) {
        DELETE_TO_EPOLL();
    } else {
        auto cb = [this, event]() { DELETE_TO_EPOLL(); };

        addTask(cb, true);
    }
}

bool Eventloop::isInLoopThread() const { return getThreadId() == m_thread_id; }
void Eventloop::addTask(const std::function<void()> &cb, bool is_wake_up) {

    {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_pending_tasks.push(cb);
    }

    if (is_wake_up) wakeup();
}

void Eventloop::dealWakeup() {}
void Eventloop::initWakeUpFdEevent() {
    m_wakeup_fd = eventfd(0, EFD_NONBLOCK);

    if (m_wakeup_fd < 0) {
        ERRORLOG(
            "failed to create event loop, eventfd create error, error info[%d]",
            errno);
        exit(0);
    }

    INFOLOG("wakeup fd = %d", m_wakeup_fd);

    m_wakeup_fd_event =
        std::unique_ptr<WakeUpFdEvent>(new WakeUpFdEvent(m_wakeup_fd));
    m_wakeup_fd_event->listen(FdEvent::IN_EVENT, [&]() {
        char buf[8];
        while (read(m_wakeup_fd, buf, 8) != -1 && errno != EAGAIN) {
        }

        DEBUGLOG("read full bytes from wakeup fd[%d]", m_wakeup_fd);
    });

    addEpollEvent(m_wakeup_fd_event.get());
}

MRPC_NAMESPACE_END