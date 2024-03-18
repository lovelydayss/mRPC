#include "log.h"
#include "net_addr.h"
#include <cstdint>

int main() {

	mrpc::configure_path = "/home/lovelydays/code/rpc/mRPC/conf/config.json";
	mrpc::Config::GetGlobalConfig();

	mrpc::IPNetAddr addr("127.0.0.1", 12345);
	DEBUGLOG("create addr %s", addr.toString().c_str());

	mrpc::IPNetAddr addr1("999.0.0.1", 12345);
	DEBUGLOG("create addr %s", addr1.toString().c_str());
	DEBUGLOG("addr check valid %d", addr1.checkValid());

	mrpc::IPNetAddr addr2("111.0.0.1", static_cast<uint16_t>(65536));
	DEBUGLOG("create addr %s", addr2.toString().c_str());
	DEBUGLOG("addr check valid %d", addr2.checkValid());

	mrpc::IPNetAddr addr3("121.0.0.1:999");
	DEBUGLOG("create addr %s", addr3.toString().c_str());

	sockaddr_in m_addr{};
	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = inet_addr("192.168.3.251");
	m_addr.sin_port = htons(121);
	mrpc::IPNetAddr addr4(m_addr);
	DEBUGLOG("create addr %s", addr4.toString().c_str());
}