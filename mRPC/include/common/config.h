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
    Config();
    std::string getLogLevel() const { return m_log_level; }
    int getIOThreadNums() const { return m_io_thread_nums; }

    static std::shared_ptr<Config> GetGlobalConfig();

  private:
    std::string m_log_level;

    int m_io_thread_nums;
};

MRPC_NAMESPACE_END

#endif