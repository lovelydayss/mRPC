#ifndef MRPC_COMMON_LOG_H
#define MRPC_COMMON_LOG_H

#include "config.h"
#include "utils.h"

#include <memory>
#include <mutex>
#include <queue>
#include <string>

MRPC_NAMESPACE_BEGIN

template <typename... Args>
std::string formatString(const char *str, Args &&...args) {

    int size = snprintf(nullptr, 0, str, args...);

    std::string result;
    if (size > 0) {
        result.resize(size);
        snprintf(&result[0], size + 1, str, args...);
    }

    return result;
}

#define DEBUGLOG(str, ...)                                                     \
    if (mrpc::Logger::GetGlobalLogger()->getLogLevel() <=                      \
        mrpc::LogLevel::Debug) {                                               \
        mrpc::Logger::GetGlobalLogger()->pushLog(                              \
            (new mrpc::LogEvent(mrpc::LogLevel::Debug))->toString() + "[" +    \
            std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" +   \
            mrpc::formatString(str, ##__VA_ARGS__) + "\n");                    \
        mrpc::Logger::GetGlobalLogger()->log();                                \
    }

#define INFOLOG(str, ...)                                                      \
    if (mrpc::Logger::GetGlobalLogger()->getLogLevel() <=                      \
        mrpc::LogLevel::Info) {                                                \
        mrpc::Logger::GetGlobalLogger()->pushLog(                              \
            (new mrpc::LogEvent(mrpc::LogLevel::Info))->toString() + "[" +     \
            std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" +   \
            mrpc::formatString(str, ##__VA_ARGS__) + "\n");                    \
        mrpc::Logger::GetGlobalLogger()->log();                                \
    }

#define ERRORLOG(str, ...)                                                     \
    if (mrpc::Logger::GetGlobalLogger()->getLogLevel() <=                      \
        mrpc::LogLevel::Error) {                                               \
        mrpc::Logger::GetGlobalLogger()->pushLog(                              \
            (new mrpc::LogEvent(mrpc::LogLevel::Error))->toString() + "[" +    \
            std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" +   \
            mrpc::formatString(str, ##__VA_ARGS__) + "\n");                    \
        mrpc::Logger::GetGlobalLogger()->log();                                \
    }

enum LogLevel { Unknown = 0, Debug = 1, Info = 2, Error = 3 };

std::string LogLevelToString(LogLevel level);
LogLevel StringToLogLevel(const std::string &log_level);

class Logger {
  public:
    explicit Logger(LogLevel level)
        : m_set_level(level) {}

    void pushLog(const std::string &msg);

    void log();

    LogLevel getLogLevel() const { return m_set_level; }

    static std::shared_ptr<Logger> GetGlobalLogger(); // 线程安全的单例模式

  private:
    LogLevel m_set_level;
    std::queue<std::string> m_buffer;

    std::mutex m_mutex;
};

class LogEvent {
  public:
    explicit LogEvent(LogLevel level)
        : m_file_line(0), m_pid(0), m_thread_id(0), m_level(level) {}

    std::string getFileName() const { return m_flie_name; }

    LogLevel getLogLevel() const { return m_level; }

    std::string toString();

  private:
    std::string m_flie_name; // 文件名称
    int32_t m_file_line;     // 行号
    int32_t m_pid;           // 进程号
    int32_t m_thread_id;     // 线程号

    LogLevel m_level; // 日志级别
};

MRPC_NAMESPACE_END

#endif