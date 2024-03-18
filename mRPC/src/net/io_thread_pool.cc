#include "io_thread_pool.h"
#include "io_thread.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

IOThreadPool::IOThreadPool(int size)
    : m_size(size) {
	m_io_threads.resize(size);
	for (auto& io_thread : m_io_threads)
		io_thread = std::make_shared<IOThread>();
}

void IOThreadPool::start() {
	for (auto& io_thread : m_io_threads)
		io_thread->start();
}

void IOThreadPool::join() {
	for (auto& io_thread : m_io_threads)
		io_thread->join();
}

const IOThread::s_ptr& IOThreadPool::getIOThread() {
	if (m_index == static_cast<int>(m_io_threads.size()) || m_index == -1)
		m_index = 0;

	return m_io_threads[m_index++];
}

MRPC_NAMESPACE_END
