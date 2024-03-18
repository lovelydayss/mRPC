#ifndef MRPC_COMMON_UTILS_H
#define MRPC_COMMON_UTILS_H

#include <memory>
#include <sys/types.h>
#include <unistd.h>

#define MRPC_NAMESPACE_BEGIN namespace mrpc {

#define MRPC_NAMESPACE_END } /* end of namespace mrpc */

MRPC_NAMESPACE_BEGIN

#define SECONDS(x) (int64_t(x) * 1000 * 1000 * 1000) // s->ns
#define MILLISECONDS(x) (int64_t(x) * 1000 * 1000)   // ms->ns
#define MICROSECONDS(x) (int64_t(x) * 1000)          // us->ns

pid_t getPid();

pid_t getThreadId();

int64_t getNowNanoseconds();

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args);

uint32_t getUInt32FromNetByte(const char* buf);

MRPC_NAMESPACE_END

#endif