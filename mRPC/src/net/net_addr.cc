#include "net_addr.h"
#include "log.h"
#include "utils.h"
#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

MRPC_NAMESPACE_BEGIN

// ip: xxx.xxx.xxx.xxx 写法
// port: 0-65535
IPNetAddr::IPNetAddr(const std::string& ip, uint16_t port) /*NOLINT*/
    : m_ip(ip)
    , m_port(port) {
	memset(&m_addr, 0, sizeof(m_addr));

	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(m_port);

	inet_pton(AF_INET, ip.c_str(), &(m_addr.sin_addr));
}

// 形如 xxx.xxx.xxx.xxx : xxxxx 写法
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

IPNetAddr::IPNetAddr(const sockaddr_in& addr)
    : m_addr(addr) {
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
	if (m_ip.empty())
		return false;

	// if (m_port < 0 || m_port > 65535)
	//     return false;

	if (inet_addr(m_ip.c_str()) == INADDR_NONE)
		return false;

	return true;
}

// ipV6: xx:xx:xx:xx:xx:xx:xx:xx 写法
// port: 0-65535
IPv6NetAddr::IPv6NetAddr(const std::string& ipv6, uint16_t port) /*NOLINT*/
    : m_ipv6(ipv6)
    , m_port(port) {
	memset(&m_addr, 0, sizeof(m_addr));

	m_addr.sin6_family = AF_INET6;
	m_addr.sin6_port = htons(m_port);

	inet_pton(AF_INET6, ipv6.c_str(), &(m_addr.sin6_addr));
}

IPv6NetAddr::IPv6NetAddr(const sockaddr_in6& addr)
    : m_addr(addr) {
	char ipv6text[INET6_ADDRSTRLEN];
	memset(ipv6text, 0, INET6_ADDRSTRLEN);
	inet_ntop(AF_INET6, &(m_addr.sin6_addr), ipv6text, sizeof(ipv6text));

	m_ipv6 = std::string(ipv6text);
	m_port = ntohs(m_addr.sin6_port);
}

sockaddr* IPv6NetAddr::getSockAddr() {
	return reinterpret_cast<sockaddr*>(&m_addr);
}

socklen_t IPv6NetAddr::getSockLen() { return sizeof(m_addr); }

int IPv6NetAddr::getFamily() { return AF_INET6; }

std::string IPv6NetAddr::toString() {
	std::string re = m_ipv6 + ":" + std::to_string(m_port);
	return re;
}

bool IPv6NetAddr::checkValid() {
	if (m_ipv6.empty())
		return false;

	// if (m_port < 0 || m_port > 65535)
	//     return false;

	if (inet_pton(AF_INET6, m_ipv6.c_str(), &(m_addr.sin6_addr)) != 1)
		return false;

	return true;
}

MRPC_NAMESPACE_END