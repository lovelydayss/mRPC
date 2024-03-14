#include "io_thread_group.h"
#include "io_thread.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

IOThreadGroup::IOThreadGroup(int size)
    : m_size(size) {
    m_io_threads.resize(size);
    for (auto& io_thread : m_io_threads)
        io_thread = std::make_shared<IOThread>();
}

void IOThreadGroup::start() {
    for (auto& io_thread : m_io_threads)
        io_thread->start();
}

void IOThreadGroup::join() {
    for (auto& io_thread : m_io_threads)
        io_thread->join();
}

std::shared_ptr<IOThread> IOThreadGroup::getIOThread() {
    if (m_index == static_cast<int>(m_io_threads.size()) || m_index == -1)
        m_index = 0;

    return m_io_threads[m_index++];
}

MRPC_NAMESPACE_END
