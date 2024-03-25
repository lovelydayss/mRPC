#include "rpc_dispatcher.h"
#include "error_code.h"
#include "rpc_controller.h"
#include "tcp_connection.h"
#include "utils.h"
#include "log.h"
#include <cstddef>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <memory>
#include <mutex>
#include <string>

MRPC_NAMESPACE_BEGIN

static RpcDispatcher::s_ptr s_ptr_rpc_dispatcher;
static std::once_flag singleton_rpc_dispatcher;

const RpcDispatcher::s_ptr& RpcDispatcher::GetGlobalRpcDispatcher() {

	std::call_once(singleton_rpc_dispatcher, []() {
		s_ptr_rpc_dispatcher = std::make_shared<RpcDispatcher>();
	});
	return s_ptr_rpc_dispatcher;
}

void RpcDispatcher::dispatch(const AbstractProtocol::s_ptr& request,
                             const AbstractProtocol::s_ptr& response,
                             TcpConnection* connection) {

	TinyPBProtocol::s_ptr req_protocol =
	    std::static_pointer_cast<TinyPBProtocol>(request);
	TinyPBProtocol::s_ptr rsp_protocol =
	    std::static_pointer_cast<TinyPBProtocol>(response);

	std::string method_full_name = req_protocol->m_method_name;
	std::string service_name;
	std::string method_name;

	// 消息 id 和服务全名原样返回
	rsp_protocol->m_msg_id = req_protocol->m_msg_id;
	rsp_protocol->m_method_name = req_protocol->m_method_name;

	// 解析服务名称
	if (!parseServiceFullName(method_full_name, service_name, method_name)) {
		setTinyPBError(rsp_protocol, ERROR_PARSE_SERVICE_NAME,
		               "parse service name error");
		return;
	}

	// 查找目标服务
	auto it = m_service_map.find(service_name);
	if (it == m_service_map.end()) {
		ERRORLOG("%s | sericve neame[%s] not found",
		         req_protocol->m_msg_id.c_str(), service_name.c_str());
		setTinyPBError(rsp_protocol, ERROR_SERVICE_NOT_FOUND,
		               "service not found");
		return;
	}

	service_s_ptr service = it->second;
	const google::protobuf::MethodDescriptor* method =
	    service->GetDescriptor()->FindMethodByName(method_name);

	if (method == nullptr) {
		ERRORLOG("%s | method neame[%s] not found in service[%s]",
		         req_protocol->m_msg_id.c_str(), method_name.c_str(),
		         service_name.c_str());
		setTinyPBError(rsp_protocol, ERROR_SERVICE_NOT_FOUND,
		               "method not found");
		return;
	}

	google::protobuf::Message* req_msg =
	    service->GetRequestPrototype(method).New();

	// 反序列化，将 pb_data 反序列化为 req_msg
	if (!req_msg->ParseFromString(req_protocol->m_pb_data)) {
		ERRORLOG("%s | deserilize error", req_protocol->m_msg_id.c_str(),
		         method_name.c_str(), service_name.c_str());
		setTinyPBError(rsp_protocol, ERROR_FAILED_DESERIALIZE,
		               "deserilize error");
		if (req_msg != NULL) {
			delete req_msg;
			req_msg = NULL;
		}
		return;
	}

	INFOLOG("%s | get rpc request[%s]", req_protocol->m_msg_id.c_str(),
	        req_msg->ShortDebugString().c_str());

	google::protobuf::Message* rsp_msg =
	    service->GetResponsePrototype(method).New();

	RpcController rpcController;
	rpcController.SetLocalAddr(connection->getLocalAddr());
	rpcController.SetPeerAddr(connection->getPeerAddr());
	rpcController.SetMsgId(req_protocol->m_msg_id);

	service->CallMethod(method, &rpcController, req_msg, rsp_msg, NULL);

	if (!rsp_msg->SerializeToString(&(rsp_protocol->m_pb_data))) {
		ERRORLOG("%s | serilize error, origin message [%s]",
		         req_protocol->m_msg_id.c_str(),
		         rsp_msg->ShortDebugString().c_str());
		setTinyPBError(rsp_protocol, ERROR_SERVICE_NOT_FOUND, "serilize error");
		return;

		if (req_msg != nullptr) {
			delete req_msg;
			req_msg = nullptr;
		}
		if (rsp_msg != nullptr) {
			delete rsp_msg;
			rsp_msg = nullptr;
		}
	}

	rsp_protocol->m_err_code = 0;
	INFOLOG("%s | dispatch success, requesut[%s], response[%s]",
	        req_protocol->m_msg_id.c_str(), req_msg->ShortDebugString().c_str(),
	        rsp_msg->ShortDebugString().c_str());

	if (req_msg != nullptr) {
		delete req_msg;
		req_msg = nullptr;
	}
	if (rsp_msg != nullptr) {
		delete rsp_msg;
		rsp_msg = nullptr;
	}
}

void RpcDispatcher::registerService(const service_s_ptr& service) {
	std::string service_name = service->GetDescriptor()->full_name();
	m_service_map[service_name] = service;
}

void RpcDispatcher::setTinyPBError(const TinyPBProtocol::s_ptr& msg,
                                   int32_t err_code,
                                   const std::string& err_info) {
	msg->m_err_code = err_code;
	msg->m_err_info = err_info;
	msg->m_err_info_len = err_info.length();
}

bool RpcDispatcher::parseServiceFullName(const std::string& full_name,
                                         std::string& service_name,
                                         std::string& method_name) {
	if (full_name.empty()) {
		ERRORLOG("full name empty%s", "");
		return false;
	}

	// 使用 . 分割服务全名
	size_t i = full_name.find_first_of('.');
	if (i == std::string::npos) {
		ERRORLOG("not find . in full name [%s]", full_name.c_str());
		return false;
	}

	service_name = full_name.substr(0, i);
	method_name = full_name.substr(i + 1, full_name.length() - i - 1);

	INFOLOG("parse sericve_name[%s] and method_name[%s] from full name [%s]",
	        service_name.c_str(), method_name.c_str(), full_name.c_str());

	return true;
}

MRPC_NAMESPACE_END
