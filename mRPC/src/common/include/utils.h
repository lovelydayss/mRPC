#ifndef MRPC_COMMON_UTILS_H
#define MRPC_COMMON_UTILS_H

#include <memory>
#include <sys/types.h>
#include <unistd.h>

#if __cplusplus >= 201703L

#define FMTLOG_HEADER_ONLY
#include "fmtlog/fmtlog.h"

#endif

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

#if __cplusplus >= 201703L

#define SETLOGLEVEL(level) fmtlog::setLogLevel(level)
#define SETLOGHEADER(header) fmtlog::setHeaderPattern(header)

#define DEBUGFMTLOG(str, ...) logd(str, ##__VA_ARGS__)
#define INFOFMTLOG(str, ...) logi(str, ##__VA_ARGS__)
#define WARNINGFMTLOG(str, ...) logw(str, ##__VA_ARGS__)
#define ERRORFMTLOG(str, ...) loge(str, ##__VA_ARGS__)

#define DEBUGFMTLOG_ONCE(str, ...) logdo(str, ##__VA_ARGS__)
#define INFOFMTLOG_ONCE(str, ...) logio(str, ##__VA_ARGS__)
#define WARNINGFMTLOG_ONCE(str, ...) logwo(str, ##__VA_ARGS__)
#define ERRORFMTLOG_ONCE(str, ...) logeo(str, ##__VA_ARGS__)

#define FMTLOGPOLL() fmtlog::poll()
#define CREATEPOLLTHREAD(t) fmtlog::startPollingThread(t)

#define SETLOGFILE(file, flag) fmtlog::setLogFile(file, flag)
#define CLOSELOGFILE() fmtlog::closeLogFile()

#else

#define SETLOGLEVEL(level)
#define SETLOGHEADER(header)

#define DEBUGFMTLOG(str, ...)
#define INFOFMTLOG(str, ...)
#define WARNINGFMTLOG(str, ...)
#define ERRORFMTLOG(str, ...)

#define DEBUGFMTLOG_ONCE(str, ...)
#define INFOFMTLOG_ONCE(str, ...)
#define WARNINGFMTLOG_ONCE(str, ...)
#define ERRORFMTLOG_ONCE(str, ...)

#define POLL()
#define CREATEPOLLTHREAD(t) 

#define SETLOGFILE(file, flag)
#define CLOSELOGFILE()

#endif

MRPC_NAMESPACE_END

#endif