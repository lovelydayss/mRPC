#ifndef MRPC_NET_RPC_RPC_DISPATCHER_H
#define MRPC_NET_RPC_RPC_DISPATCHER_H

#include "abstract_protocol.h"
#include "tcp_connection.h"
#include "tinypb_protocol.h"
#include "utils.h"
#include <google/protobuf/service.h>
#include <map>
#include <memory>

MRPC_NAMESPACE_BEGIN

class RpcDispatcher {

public:
	using s_ptr = std::shared_ptr<RpcDispatcher>;
	using service_s_ptr = std::shared_ptr<google::protobuf::Service>;

public:
	RpcDispatcher() = default;
	~RpcDispatcher() = default;

	// 任务分发
	void dispatch(const AbstractProtocol::s_ptr& request,
	              const AbstractProtocol::s_ptr& response,
	              const TcpConnection::s_ptr& connection);

	// 注册服务
	void registerService(const service_s_ptr& service);

	// 设置错误信息
	static void setTinyPBError(const TinyPBProtocol::s_ptr& msg,
	                           int32_t err_code, const std::string& err_info);

public:
	static const RpcDispatcher::s_ptr& GetGlobalRpcDispatcher();

private:
	// 服务名称解析
	static bool parseServiceFullName(const std::string& full_name,
	                                 std::string& service_name,
	                                 std::string& method_name);

private:
	std::map<std::string, service_s_ptr> m_service_map;
};

MRPC_NAMESPACE_END

#endif