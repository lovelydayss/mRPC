#include "config.h"
#include "utils.h"
#include "order.pb.h"
#include "rpc_channel.h"
#include "rpc_closure.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <google/protobuf/service.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {

	mrpc::configure_path = "/data/home/linearzhang/workspace/mproject/rpc/mRPC/conf/config.json";
	mrpc::Config::GetGlobalConfig();

	NEWRPCCHANNEL("127.0.0.1:12346", channel);

	NEWMESSAGE(makeOrderRequest, request);
	NEWMESSAGE(makeOrderResponse, response);

	request->set_price(100);
	request->set_goods("apple");

	NEWRPCCONTROLLER(controller);
	controller->SetMsgId("99998888");
	controller->SetTimeout(SECONDS(100000));

	std::shared_ptr<mrpc::RpcClosure> closure = std::make_shared<
	    mrpc::RpcClosure>([request, response, channel, controller]() mutable {
		if (controller->GetErrorCode() == 0) {
			INFOFMTLOG("call rpc success, request[{}], response[{}]",
			        request->ShortDebugString().c_str(),
			        response->ShortDebugString().c_str());
			// 执行业务逻辑
			if (response->order_id() == "xxx") {
				// xx
			}
		} else {
			ERRORFMTLOG(
			    "call rpc failed, request[{}], error code[{}], error info[{}]",
			    request->ShortDebugString().c_str(), controller->GetErrorCode(),
			    controller->GetErrorInfo().c_str());
		}

		INFOFMTLOG("now exit eventloop");
		channel->getTcpClient()->stop();
		channel.reset();
	});

	CALLRPRC("127.0.0.1:12346", makeOrder, controller, request, response,
	         closure);

	INFOFMTLOG("test_rpc_channel end");

	return 0;
}