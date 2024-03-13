#ifndef MRPC_NET_IO_THREAD_H
#define MRPC_NET_IO_THREAD_H

#include "eventloop.h"
#include "log.h"
#include "utils.h"
#include <memory>
#include <semaphore.h>
#include <sys/types.h>
#include <thread>

MRPC_NAMESPACE_BEGIN

class IOThread {
  public:
    IOThread();
    ~IOThread();

    std::shared_ptr<Eventloop> getEventLoop();

    void start();
    void join();
    bool joinable();

  public:
    static void* Main(void* arg);

  private:
    pid_t m_thread_id{-1};
    std::unique_ptr<std::thread> m_thread{nullptr};
    std::shared_ptr<Eventloop> m_eventloop{nullptr};

    sem_t m_init_semaphore{}, m_start_semaphore{};
};

MRPC_NAMESPACE_END

#endif