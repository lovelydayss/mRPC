#include "utils.h"

#include <memory>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

MRPC_NAMESPACE_BEGIN

static int g_pid = 0;

static thread_local int g_thread_id = 0;

pid_t getPid() {
    if (g_pid != 0) {
        return g_pid;
    }
    return getpid();
}

pid_t getThreadId() {
    if (g_thread_id != 0) {
        return g_thread_id;
    }
    return static_cast<pid_t>(syscall(SYS_gettid));
}

template<typename T, typename... Args>
typename std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

MRPC_NAMESPACE_END