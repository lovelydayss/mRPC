#include "net_addr.h"
#include "log.h"
#include "utils.h"
#include <cstdint>
#include <cstring>
#include <string>

MRPC_NAMESPACE_BEGIN

// ip: xxx.xxx.xxx.xxx 写法
// port: 0-65535
IPNetAddr::IPNetAddr(const std::string& ip, uint16_t port)
    : m_ip(ip)
    , m_port(port) {
    memset(&m_addr, 0, sizeof(m_addr));

    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
    m_addr.sin_port = htons(m_port);
}

// 形如 xxx.xxx.xxx.xxx:xx 写法
IPNetAddr::IPNetAddr(const std::string& addr) {
    size_t i = addr.find_first_of(':');
    if (i == std::string::npos) {
        ERRORLOG("invalid ipv4 addr %s", addr.c_str());
        return;
    }
    m_ip = addr.substr(0, i);
    m_port = static_cast<uint16_t>(
        std::stoul(addr.substr(i + 1, addr.size() - i - 1)));

    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
    m_addr.sin_port = htons(m_port);
}

IPNetAddr::IPNetAddr(const sockaddr_in& addr) : m_addr(addr) {
    m_ip = std::string(inet_ntoa(m_addr.sin_addr));
    m_port = ntohs(m_addr.sin_port);
}

sockaddr* IPNetAddr::getSockAddr() {
    return reinterpret_cast<sockaddr*>(&m_addr);
}

socklen_t IPNetAddr::getSockLen() { return sizeof(m_addr); }

int IPNetAddr::getFamily() { return AF_INET; }

std::string IPNetAddr::toString() {
    std::string re = m_ip + ":" + std::to_string(m_port);
    return re;
}

bool IPNetAddr::checkValid() {
    if (m_ip.empty()) return false;

    // if (m_port < 0 || m_port > 65535)
    //     return false;

    if (inet_addr(m_ip.c_str()) == INADDR_NONE) return false;

    return true;
}

MRPC_NAMESPACE_END