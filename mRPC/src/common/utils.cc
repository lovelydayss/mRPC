#include "utils.h"
#include <chrono>
#include <cstring>
#include <memory>
#include <netinet/in.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

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

int64_t getNowNanoseconds() {

	std::chrono::high_resolution_clock::time_point now =
	    std::chrono::high_resolution_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::nanoseconds>(
	    now.time_since_epoch());

	return ms.count();
}

template <typename T, typename... Args>
typename std::unique_ptr<T> make_unique(Args&&... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

uint32_t getUInt32FromNetByte(const char* buf) {
	uint32_t re;
	memcpy(&re, buf, sizeof(re));
	return ntohl(re);
}

MRPC_NAMESPACE_END