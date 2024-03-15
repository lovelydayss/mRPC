#include "config.h"

#include <fstream>
#include <json/json.h>
#include <memory>
#include <mutex>
#include <string>

MRPC_NAMESPACE_BEGIN

std::string configure_path;
static Config::s_ptr s_ptr_config = nullptr;
static std::once_flag singleton_config;

Config::Config() {

    Json::Reader reader;
    Json::Value root;

    std::ifstream in(configure_path, std::ios::binary);
    if (!in.is_open()) throw "Wrong Configure File Path";

    if (reader.parse(in, root)) {
        m_log_level = root["rpc_log"]["log_level"].asString();
        m_io_thread_nums = root["server"]["io_thread_nums"].asInt();
        m_fd_event_nums = root["server"]["fd_event_nums"].asInt();
        m_connection_buffer_size =
            root["server"]["connection_bufffer_size"].asInt();
    }
}

const Config::s_ptr& Config::GetGlobalConfig() {

    std::call_once(singleton_config,
                   [&]() { s_ptr_config = std::make_shared<Config>(); });

    return s_ptr_config;
}

MRPC_NAMESPACE_END