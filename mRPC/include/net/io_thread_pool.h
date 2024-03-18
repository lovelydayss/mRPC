#ifndef MRPC_NET_IO_THREAD_POOL_H
#define MRPC_NET_IO_THREAD_POOL_H

#include "io_thread.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

class IOThreadPool {
public:
	using s_ptr = std::shared_ptr<IOThreadPool>;

public:
	explicit IOThreadPool(int size);

	~IOThreadPool() = default;

	void start();
	void join();

	const IOThread::s_ptr& getIOThread();

private:
	int m_size{0};
	std::vector<IOThread::s_ptr> m_io_threads;

	int m_index{0};
};

MRPC_NAMESPACE_END

#endif
