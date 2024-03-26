#include "abstract_protocol.h"
#include "config.h"
#include "net_addr.h"
#include "tcp_client.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <memory>
#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

void test_connect() {

	// 调用 conenct 连接 server
	// wirte 一个字符串
	// 等待 read 返回结果

	int fd = socket(AF_INET, SOCK_STREAM, 0);

	if (fd < 0) {
		ERRORFMTLOG("invalid fd {}", fd);
		exit(0);
	}

	mrpc::IPNetAddr::s_ptr server_addr =
	    std::make_shared<mrpc::IPNetAddr>("127.0.0.1", 12346);
	int ret = connect(fd, server_addr->getSockAddr(), server_addr->getSockLen());
	DEBUGFMTLOG("connect success : ret = {}", ret);

	std::string msg = "hello mrpc!";

	ret = static_cast<int>(write(fd, msg.c_str(), msg.length()));
	DEBUGFMTLOG("success write {} bytes, [{}]", ret, msg.c_str());

	char buf[100];
	ret = static_cast<int>(read(fd, buf, 100));
	DEBUGFMTLOG("success read {} bytes, [{}]", ret, std::string(buf).c_str());
}

void test_tcp_client() {

	mrpc::IPNetAddr::s_ptr addr =
	    std::make_shared<mrpc::IPNetAddr>("127.0.0.1", 12346);
	mrpc::TcpClient client(addr);
	
	client.asyncConnect([&addr, &client]() {

		mrpc::AbstractProtocol::s_ptr test_s =
		    std::make_shared<mrpc::AbstractProtocol>();
		test_s->m_msg_id = "1213213213123";

		client.writeMessage(
		    test_s, [](const mrpc::AbstractProtocol::s_ptr&) mutable {
		    });
	});
}

int main() {

	mrpc::configure_path = "/home/lovelydays/code/rpc/mRPC/conf/config.json";
	mrpc::Config::GetGlobalConfig();

	// test_connect();

	test_tcp_client();

	return 0;
}