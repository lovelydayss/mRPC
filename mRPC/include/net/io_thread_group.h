#ifndef MRPC_NET_IO_THREAD_GROUP_H
#define MRPC_NET_IO_THREAD_GROUP_H

#include "io_thread.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

class IOThreadGroup {
public:
	using s_ptr = std::shared_ptr<IOThreadGroup>;

public:
	explicit IOThreadGroup(int size);

	~IOThreadGroup() = default;

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
