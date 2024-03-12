#ifndef MRPC_COMMON_UTILS_H
#define MRPC_COMMON_UTILS_H

#include <sys/types.h>
#include <unistd.h>
#include <memory>

#define MRPC_NAMESPACE_BEGIN namespace mrpc {

#define MRPC_NAMESPACE_END } /* end of namespace mrpc */

MRPC_NAMESPACE_BEGIN

pid_t getPid();

pid_t getThreadId();

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args);

MRPC_NAMESPACE_END

#endif