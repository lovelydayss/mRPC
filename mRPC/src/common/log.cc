#include "log.h"
#include "config.h"
#include "utils.h"
#include <bits/types/time_t.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

MRPC_NAMESPACE_BEGIN

static Logger::s_ptr s_ptr_logger = nullptr;
static std::once_flag singleton_logger;

Logger::s_ptr Logger::GetGlobalLogger() {

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

LogLevel StringToLogLevel(const std::string& log_level) {

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

	std::chrono::system_clock::time_point now =
	    std::chrono::system_clock::now();
	std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
	              now.time_since_epoch()) %
	          1000;
	auto cs = std::chrono::duration_cast<std::chrono::microseconds>(
	              now.time_since_epoch()) %
	          1000 % 1000 % 1000;

	m_pid = getPid();
	m_thread_id = getThreadId();

	std::stringstream ss;

	ss << "[" << LogLevelToString(m_level) << "]\t"
	   << "[" << std::put_time(localtime(&now_time_t), "%Y-%m-%d %H:%M:%S")
	   << ":" << ms.count() << "." << cs.count() << "]\t"
	   << "[" << m_pid << ":" << m_thread_id << "]\t";

	return ss.str();
}

void Logger::pushLog(const std::string& msg) {

	std::lock_guard<std::mutex> lk(m_mutex);
	m_buffer.push(msg);
}

void Logger::log() {

	std::queue<std::string> tmp;

	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_buffer.swap(tmp);
	}

	while (!tmp.empty()) {
		std::string msg = tmp.front();
		tmp.pop();
		printf("%s", msg.c_str());
	}
}

MRPC_NAMESPACE_END
