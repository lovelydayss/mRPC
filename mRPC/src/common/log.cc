#include "utils.h"
#include "config.h"
#include "log.h"

#include <bits/types/time_t.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

MRPC_NAMESPACE_BEGIN

static std::shared_ptr<Logger> s_ptr_logger = nullptr;
static std::once_flag singleton_logger;

std::shared_ptr<Logger> Logger::GetGlobalLogger() {

    std::call_once(singleton_logger, [&]() {
        LogLevel global_log_level =
            StringToLogLevel(Config::GetGlobalConfig()->getLogLevel());
        printf("Init log level [%s]\n",
               LogLevelToString(global_log_level).c_str());
        s_ptr_logger = std::make_shared<Logger>(global_log_level);
    });

    return s_ptr_logger;
}

std::string LogLevelToString(LogLevel level) {
    switch (level) {
    case Debug:
        return "DEBUG";
    case Info:
        return "INFO";
    case Error:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

LogLevel StringToLogLevel(const std::string &log_level) {

    if (log_level == "DEBUG")
        return Debug;
    else if (log_level == "INFO")
        return Info;
    else if (log_level == "ERROR")
        return Error;
    else
        return Unknown;
}

std::string LogEvent::toString() {

    time_t cur_time_t = std::chrono::system_clock::to_time_t(
        std::chrono::high_resolution_clock::now());
    struct tm cutTM = {0};

    m_pid = getPid();
    m_thread_id = getThreadId();

    std::stringstream ss;

    ss << "[" << LogLevelToString(m_level) << "]\t"
       << "["
       << std::put_time(localtime_r(&cur_time_t, &cutTM), "%y-%m-%d %H:%M:%S")
       << "]\t"
       << "[" << m_pid << ":" << m_thread_id << "]\t";

    return ss.str();
}

void Logger::pushLog(const std::string &msg) {

    std::lock_guard<std::mutex> lk(m_mutex);
    m_buffer.push(msg);
}

void Logger::log() {

    std::queue<std::string> tmp;

    {
        std::lock_guard<std::mutex> lk(m_mutex);
        tmp = m_buffer;
        m_buffer.swap(tmp);
    }

    while (!tmp.empty()) {
        std::string msg = tmp.front();
        tmp.pop();
        printf("%s", msg.c_str());
    }
}

MRPC_NAMESPACE_END
