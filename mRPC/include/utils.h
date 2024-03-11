#ifndef MRPC_COMMON_UTILS_H
#define MRPC_COMMON_UTILS_H

#include <sys/types.h>
#include <unistd.h>

#define MRPC_NAMESPACE_BEGIN namespace mrpc {

#define MRPC_NAMESPACE_END } /* end of namespace mrpc */

MRPC_NAMESPACE_BEGIN

pid_t getPid();

pid_t getThreadId();

MRPC_NAMESPACE_END

#endif