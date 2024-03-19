#ifndef MRPC_NET_TCP_NET_ADDR_H
#define MRPC_NET_TCP_NET_ADDR_H

#include "utils.h"
#include <arpa/inet.h>
#include <memory>
#include <netinet/in.h>
#include <string>

MRPC_NAMESPACE_BEGIN

class NetAddr {
public:
	using s_ptr = std::shared_ptr<NetAddr>;

public:
	virtual sockaddr* getSockAddr() = 0;
	virtual socklen_t getSockLen() = 0;
	virtual int getFamily() = 0;

	virtual std::string toString() = 0;
	virtual bool checkValid() = 0;
};

class IPNetAddr : public NetAddr {
public:
	using s_ptr = std::shared_ptr<IPNetAddr>;

public:
	IPNetAddr(const std::string& ip, uint16_t port);
	explicit IPNetAddr(const std::string& addr);
	explicit IPNetAddr(const sockaddr_in& addr);

	sockaddr* getSockAddr() override;
	socklen_t getSockLen() override;
	int getFamily() override;

	std::string toString() override;
	bool checkValid() override;

private:
	std::string m_ip;
	uint16_t m_port{0}; // uint16_t 限定了 0-65535

	sockaddr_in m_addr{};
};


class IPv6NetAddr : public NetAddr {
public:
	using s_ptr = std::shared_ptr<IPv6NetAddr>;

public:
	IPv6NetAddr(const std::string& ipv6, uint16_t port);
	explicit IPv6NetAddr(const sockaddr_in6& addr);

	sockaddr* getSockAddr() override;
	socklen_t getSockLen() override;
	int getFamily() override;

	std::string toString() override;
	bool checkValid() override;

private:
	std::string m_ipv6;
	uint16_t m_port{0}; 	// uint16_t 限定了 0-65535

	sockaddr_in6 m_addr{};
};


MRPC_NAMESPACE_END

#endif
