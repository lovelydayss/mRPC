#ifndef MRPC_NET_FD_EVENT_H
#define MRPC_NET_FD_EVENT_H

#include "utils.h"
#include <cstring>
#include <functional>
#include <sys/epoll.h>

MRPC_NAMESPACE_BEGIN

class FdEvent {
public:
	using s_ptr = std::shared_ptr<FdEvent>;

public:
	enum TriggerEvent {
		IN_EVENT = EPOLLIN,
		OUT_EVENT = EPOLLOUT,
		ERROR_EVENT = EPOLLERR
	};

	FdEvent() { memset(&m_listen_events, 0, sizeof(m_listen_events)); }
	explicit FdEvent(int fd);

	~FdEvent() = default;

	// 监听读写事件，给套接字绑定回调函数
	void listen(TriggerEvent event_type, const std::function<void()>& callback,
	            const std::function<void()>& error_callback = nullptr);
	// 取消监听
	void cancel(TriggerEvent event_type);

	const std::function<void()>&
	handler(TriggerEvent event_type); // 根据套接字类型返回对应回调函数

	int getFd() const { return m_fd; }
	const epoll_event& getEpollEvent() const { return m_listen_events; }

	void setNonBlock() const;
	void setErrorCallback(const std::function<void()>& callback);

protected:
	int m_fd{-1};                  // 事件对应套接字
	epoll_event m_listen_events{}; // epoll 事件

	// 回调函数
	std::function<void()> m_read_callback{nullptr};
	std::function<void()> m_write_callback{nullptr};
	std::function<void()> m_error_callback{nullptr};
};

MRPC_NAMESPACE_END

#endif
