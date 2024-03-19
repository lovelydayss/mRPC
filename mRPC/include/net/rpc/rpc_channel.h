#ifndef MRPC_NET_RPC_RPC_CHANNEL_H
#define MRPC_NET_RPC_RPC_CHANNEL_H

#include "net_addr.h"
#include "rpc_controller.h"
#include "tcp_client.h"
#include "timer_event.h"
#include "utils.h"
#include <google/protobuf/service.h>
#include <memory>

MRPC_NAMESPACE_BEGIN

#define NEWMESSAGE(type, var_name) \
	std::shared_ptr<type> var_name = std::make_shared<type>();

#define NEWRPCCONTROLLER(var_name)                  \
	std::shared_ptr<mrpc::RpcController> var_name = \
	    std::make_shared<mrpc::RpcController>();

#define NEWRPCCHANNEL(addr, var_name)            \
	std::shared_ptr<mrpc::RpcChannel> var_name = \
	    std::make_shared<mrpc::RpcChannel>(      \
	        std::make_shared<mrpc::IPNetAddr>(addr));

#define CALLRPRC(addr, method_name, controller, request, response, closure) \
	{                                                                       \
		NEWRPCCHANNEL(addr, channel);                                       \
		channel->Init(controller, request, response, closure);              \
		Order_Stub(channel.get())                                           \
		    .method_name((controller).get(), (request).get(),               \
		                 (response).get(), (closure).get());                \
	}

class RpcChannel : public google::protobuf::RpcChannel,
                   public std::enable_shared_from_this<RpcChannel> {

public:
	typedef std::shared_ptr<RpcChannel> s_ptr;
	typedef std::shared_ptr<google::protobuf::RpcController> controller_s_ptr;
	typedef std::shared_ptr<google::protobuf::Message> message_s_ptr;
	typedef std::shared_ptr<google::protobuf::Closure> closure_s_ptr;

public:
	explicit RpcChannel(const NetAddr::s_ptr& peer_addr)
	    : m_peer_addr(peer_addr) {
		m_client = std::make_shared<TcpClient>(m_peer_addr);
	}

	~RpcChannel() { INFOLOG("~RpcChannel"); }

	void Init(const controller_s_ptr& controller, const message_s_ptr& req,
	          const message_s_ptr& res, const closure_s_ptr& done);

	void CallMethod(const google::protobuf::MethodDescriptor* method,
	                google::protobuf::RpcController* controller,
	                const google::protobuf::Message* request,
	                google::protobuf::Message* response,
	                google::protobuf::Closure* done) override;

	google::protobuf::RpcController* getController() {
		return m_controller.get();
	}

	google::protobuf::Message* getRequest() { return m_request.get(); }

	google::protobuf::Message* getResponse() { return m_response.get(); }

	google::protobuf::Closure* getClosure() { return m_closure.get(); }

	TcpClient* getTcpClient() { return m_client.get(); }

	const TimerEvent::s_ptr& getTimerEvent() { return m_timer_event; }

private:
	NetAddr::s_ptr m_peer_addr{nullptr};
	NetAddr::s_ptr m_local_addr{nullptr};

	controller_s_ptr m_controller{nullptr};
	message_s_ptr m_request{nullptr};
	message_s_ptr m_response{nullptr};
	closure_s_ptr m_closure{nullptr};

	bool m_is_init{false};

	TcpClient::s_ptr m_client{nullptr};
	TimerEvent::s_ptr m_timer_event{nullptr};
};

MRPC_NAMESPACE_END

#endif