#include "fd_event_pool.h"
#include "config.h"
#include "fd_event.h"
#include "utils.h"
#include <cstddef>
#include <memory>
#include <mutex>

MRPC_NAMESPACE_BEGIN

static FdEventPool::s_ptr s_ptr_FdEventPool = nullptr;
static std::once_flag singleton_FdEventPool;

FdEvent::s_ptr FdEventPool::getFdEvent(int fd) {

	std::lock_guard<std::mutex> lk(m_mutex);

	if (static_cast<size_t>(fd) < m_fd_group.size())
		return m_fd_group[fd];

	int new_size = static_cast<int>(fd * 1.5);

	for (int i = static_cast<int>(m_fd_group.size()); i < new_size; ++i) {
		m_fd_group.push_back(std::make_shared<FdEvent>(i));
	}

	return m_fd_group[fd];
}

const FdEventPool::s_ptr& FdEventPool::GetGlobalFdEventPool() {

	std::call_once(singleton_FdEventPool, []() {
		s_ptr_FdEventPool = std::make_shared<FdEventPool>();

		for (int i = 0; i < Config::GetGlobalConfig()->getFdEventNums(); i++) {
			s_ptr_FdEventPool->m_fd_group.push_back(
			    std::make_shared<FdEvent>(i));
		}
	});

	return s_ptr_FdEventPool;
}

MRPC_NAMESPACE_END