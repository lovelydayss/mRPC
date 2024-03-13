#include "io_thread.h"
#include "eventloop.h"
#include "utils.h"
#include <cassert>
#include <memory>
#include <semaphore.h>
#include <thread>

MRPC_NAMESPACE_BEGIN

IOThread::IOThread() {

    int ret = sem_init(&m_init_semaphore, 0, 0);
    assert(ret == 0);

    ret = sem_init(&m_start_semaphore, 0, 0);
    assert(ret == 0);

    m_thread =
        std::unique_ptr<std::thread>(new std::thread(&IOThread::Main, this));
    sem_wait(&m_init_semaphore); // 等待执行完 Main 函数前置操作

    DEBUGLOG("IOThread [%d] create success", m_thread_id);
}

IOThread::~IOThread() {
    m_eventloop->stop();

    sem_destroy(&m_init_semaphore);
    sem_destroy(&m_start_semaphore);

    m_thread->join();
}

std::shared_ptr<Eventloop> IOThread::getEventLoop() { return m_eventloop; }

void IOThread::start() {
    DEBUGLOG("Now invoke IOThread %d", m_thread_id);
    sem_post(&m_start_semaphore);
}

void IOThread::join() { m_thread->join(); }

bool IOThread::joinable() { return m_thread->joinable(); }

void* IOThread::Main(void* arg) {

    auto thread = static_cast<IOThread*>(arg);

    thread->m_eventloop = Eventloop::GetThreadLocalEventloop();
    thread->m_thread_id = getThreadId();

    sem_post(&thread->m_init_semaphore); // 唤醒等待的线程

    // 让 IO 线程等待，直到 start() 中主动的调用

    DEBUGLOG("IOThread %d created, wait start semaphore", thread->m_thread_id);
    sem_wait(&thread->m_start_semaphore);
    
    DEBUGLOG("IOThread %d start loop ", thread->m_thread_id);
    Eventloop::GetThreadLocalEventloop()->loop();                       // 启动循环
    DEBUGLOG("IOThread %d end loop ", thread->m_thread_id);

    return nullptr;
}

MRPC_NAMESPACE_END
