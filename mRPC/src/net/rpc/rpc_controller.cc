#include "rpc_controller.h"
#include "utils.h"

MRPC_NAMESPACE_BEGIN

void RpcController::Reset() {
	m_error_code = 0;
	m_error_info = "";
	m_msg_id = "";
	m_is_failed = false;
	m_is_cancled = false;
	m_local_addr = nullptr;
	m_peer_addr = nullptr;
	m_timeout = MILLISECONDS(1000);
}

MRPC_NAMESPACE_END
