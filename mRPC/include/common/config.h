#ifndef MRPC_COMMON_CONFIG_H
#define MRPC_COMMON_CONFIG_H

#include "utils.h"
#include <map>
#include <memory>
#include <string>

MRPC_NAMESPACE_BEGIN

extern std::string configure_path;

class Config {
public:
	using s_ptr = std::shared_ptr<Config>;

public:
	Config();
	std::string getLogLevel() const { return m_log_level; }
	int getIOThreadNums() const { return m_io_thread_nums; }
	int getFdEventNums() const { return m_fd_event_nums; }
	int getConnectionBufferSize() const { return m_connection_buffer_size; }

	static const Config::s_ptr& GetGlobalConfig();

private:
	std::string m_log_level{"DEBUG"};

	int m_io_thread_nums{1};
	int m_fd_event_nums{128};
	int m_connection_buffer_size{128};
};

MRPC_NAMESPACE_END

#endif