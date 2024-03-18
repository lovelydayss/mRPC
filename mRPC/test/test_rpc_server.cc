#include "config.h"
#include "log.h"
#include "net_addr.h"
#include "order.pb.h"
#include "rpc_dispatcher.h"
#include "tcp_server.h"
#include <google/protobuf/service.h>
#include <memory>

class OrderImpl : public Order {
public:
	void makeOrder(google::protobuf::RpcController* controller,
	               const ::makeOrderRequest* request,
	               ::makeOrderResponse* response,
	               ::google::protobuf::Closure* done) {
		// DEBUGLOG("start sleep 5s");
		// sleep(5);
		// DEBUGLOG("end sleep 5s");
		if (request->price() < 10) {
			response->set_ret_code(-1);
			response->set_res_info("short balance");
			return;
		}
		response->set_order_id("20230514");
	}
};

int main() {

	mrpc::configure_path = "/home/lovelydays/code/rpc/mRPC/conf/config.json";
	mrpc::Config::GetGlobalConfig();

	std::shared_ptr<OrderImpl> service = std::make_shared<OrderImpl>();
	mrpc::RpcDispatcher::GetRpcDispatcher()->registerService(service);

	mrpc::IPNetAddr::s_ptr addr =
	    std::make_shared<mrpc::IPNetAddr>("127.0.0.1", 12346);

	DEBUGLOG("create addr %s", addr->toString().c_str());

	mrpc::TcpServer tcp_server(addr);

	tcp_server.start();

	return 0;
}