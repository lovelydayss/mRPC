#ifndef MRPC_NET_RPC_RPC_CLOSURE_H
#define MRPC_NET_RPC_RPC_CLOSURE_H

#include "utils.h"
#include <functional>
#include <google/protobuf/stubs/callback.h>

MRPC_NAMESPACE_BEGIN

class RpcClosure : public google::protobuf::Closure {
public:
	explicit RpcClosure(const std::function<void()>& cb)
	    : m_cb(cb) {}

	void Run() override {
		if (m_cb != nullptr) {
			m_cb();
		}
	}

private:
	std::function<void()> m_cb{nullptr};
};

MRPC_NAMESPACE_END

#endif