#ifndef MRPC_NET_CODEC_ABSTRACT_PROTOCOL_H
#define MRPC_NET_CODEC_ABSTRACT_PROTOCOL_H

#include "utils.h"
#include <string>

MRPC_NAMESPACE_BEGIN

enum ProtocolType{HTTP_PROTOCOL, TINYPB_PROTOCOL};

class AbstractProtocol {
public:
	using s_ptr = std::shared_ptr<AbstractProtocol>;

public:
	std::string m_msg_id; // massage id 标识请求和响应
};

MRPC_NAMESPACE_END

#endif