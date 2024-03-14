#ifndef MRPC_NET_IO_THREAD_GROUP_H
#define MRPC_NET_IO_THREAD_GROUP_H

#include "io_thread.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

class IOThreadGroup {

  public:
    explicit IOThreadGroup(int size);

    ~IOThreadGroup() = default;

    void start();
    void join();

    std::shared_ptr<IOThread> getIOThread();

  private:
    int m_size{0};
    std::vector<std::shared_ptr<IOThread>> m_io_threads;

    int m_index{0};
};

MRPC_NAMESPACE_END

#endif
