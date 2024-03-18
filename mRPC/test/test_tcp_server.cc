#include "log.h"
#include "net_addr.h"
#include "tcp_server.h"
#include <memory>

int main() {

	mrpc::configure_path = "/home/lovelydays/code/rpc/mRPC/conf/config.json";
	mrpc::Config::GetGlobalConfig();

	mrpc::IPNetAddr::s_ptr addr =
	    std::make_shared<mrpc::IPNetAddr>("127.0.0.1", 12346);
	DEBUGLOG("create addr %s", addr->toString().c_str());

	mrpc::TcpServer tcp_server(addr);
	tcp_server.start();
}