#ifndef MRPC_COMMON_UTILS_H
#define MRPC_COMMON_UTILS_H

#include <memory>
#include <sys/types.h>
#include <unistd.h>

#define MRPC_NAMESPACE_BEGIN namespace mrpc {

#define MRPC_NAMESPACE_END } /* end of namespace mrpc */

MRPC_NAMESPACE_BEGIN

pid_t getPid();

pid_t getThreadId();

int64_t getNowMs();

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args);

uint32_t getUInt32FromNetByte(const char* buf);

MRPC_NAMESPACE_END

#endif