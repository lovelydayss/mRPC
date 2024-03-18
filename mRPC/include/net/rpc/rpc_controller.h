#ifndef MRPC_NET_RPC_RPC_CONTROLLER_H
#define MRPC_NET_RPC_RPC_CONTROLLER_H

#include "net_addr.h"
#include "utils.h"
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>
#include <string>
#include <sys/types.h>

MRPC_NAMESPACE_BEGIN

class RpcController : public google::protobuf::RpcController {

public:
	RpcController() = default;
	~RpcController() = default;

	void Reset() override;

	bool Failed() const override { return m_is_failed; }

	std::string ErrorText() const override { return m_error_info; }

	void SetFailed(const std::string& reason) override {
		m_error_info = reason;
	}

	void StartCancel() override { m_is_cancled = true; }

	bool IsCanceled() const override { return m_is_cancled; }

	void NotifyOnCancel(google::protobuf::Closure* callback) override {}

	void SetError(uint32_t error_code, const std::string& error_info) {
		m_error_code = error_code;
		m_error_info = error_info;
		m_is_failed = true;
	}

	uint32_t GetErrorCode() const { return m_error_code; }

	const std::string& GetErrorInfo() const { return m_error_info; }

	void SetMsgId(const std::string& msg_id) { m_msg_id = msg_id; }

	const std::string& GetMsgId() const { return m_msg_id; }

	void SetLocalAddr(const NetAddr::s_ptr& addr) { m_local_addr = addr; }

	void SetPeerAddr(const NetAddr::s_ptr& addr) { m_peer_addr = addr; }

	const NetAddr::s_ptr& GetLocalAddr() const { return m_local_addr; }

	const NetAddr::s_ptr& GetPeerAddr() const { return m_peer_addr; }

	void SetTimeout(int64_t timeout) { m_timeout = timeout; }

	int64_t GetTimeout() const { return m_timeout; }

private:
	uint32_t m_error_code{0};
	std::string m_error_info;

	std::string m_msg_id;

	NetAddr::s_ptr m_local_addr;
	NetAddr::s_ptr m_peer_addr;

	int64_t m_timeout{MILLISECONDS(1000)}; // 1000 ms

	bool m_is_failed{false};
	bool m_is_cancled{false};
};

MRPC_NAMESPACE_END

#endif