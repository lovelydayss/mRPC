#include "utils.h"

#include <memory>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <chrono>

MRPC_NAMESPACE_BEGIN

static int g_pid = 0;

static thread_local int t_thread_id = 0;

pid_t getPid() {
    if (g_pid != 0) {
        return g_pid;
    }
    return getpid();
}

pid_t getThreadId() {
    if (t_thread_id != 0) {
        return t_thread_id;
    }
    return static_cast<pid_t>(syscall(SYS_gettid));
}

int64_t getNowMs() {

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    return ms.count();

}

template<typename T, typename... Args>
typename std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

MRPC_NAMESPACE_END