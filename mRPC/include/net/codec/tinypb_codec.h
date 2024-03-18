#ifndef MRPC_NET_CODEC_TINYPB_CODEC_H
#define MRPC_NET_CODEC_TINYPB_CODEC_H

#include "utils.h"
#include "abstract_codec.h"
#include "tinypb_protocol.h"
#include <cstdint>
#include <google/protobuf/stubs/port.h>

MRPC_NAMESPACE_BEGIN

class TinyPBCodec : public AbstractCodec {

public:
	TinyPBCodec() = default;
	~TinyPBCodec() = default;

	// 将 message 对象转化为字节流，写入到 buffer
	void encode(std::vector<AbstractProtocol::s_ptr>& messages,
	            TcpBuffer::s_ptr out_buffer) override;

	// 将 buffer 里面的字节流转换为 message 对象
	void decode(std::vector<AbstractProtocol::s_ptr>& out_messages,
	            TcpBuffer::s_ptr buffer) override;

private:
	static const char* encodeTinyPB(TinyPBProtocol::s_ptr, uint32_t& len);
};

MRPC_NAMESPACE_END

#endif