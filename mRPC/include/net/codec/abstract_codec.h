#ifndef MRPC_NET_CODEC_ABSTRACT_CODEC_H
#define MRPC_NET_CODEC_ABSTRACT_CODEC_H

#include "abstract_protocol.h"
#include "tcp_buffer.h"
#include "utils.h"
#include <memory>
#include <vector>

MRPC_NAMESPACE_BEGIN

class AbstractCodec {
public:
	using s_ptr = std::shared_ptr<AbstractCodec>;

public:
	AbstractCodec() = default;
	virtual ~AbstractCodec() = default;

	// 将 message 对象转化为字节流，写入到 buffer
	virtual void encode(std::vector<AbstractProtocol::s_ptr>& messages,
	                    TcpBuffer::s_ptr out_buffer) = 0;

	// 将 buffer 里面的字节流转换为 message 对象
	virtual void decode(std::vector<AbstractProtocol::s_ptr>& out_messages,
	                    TcpBuffer::s_ptr buffer) = 0;
};

MRPC_NAMESPACE_END

#endif