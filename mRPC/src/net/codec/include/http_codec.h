#ifndef MRPC_NET_CODEC_HTTP_CODEC_H
#define MRPC_NET_CODEC_HTTP_CODEC_H

#include "abstract_codec.h"
#include "abstract_protocol.h"
#include "http_protocol.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

class HTTPCodec : public AbstractCodec {
public:
	using s_ptr = std::shared_ptr<AbstractCodec>;

public:
	HTTPCodec() = default;
	~HTTPCodec() = default;

	// 将 message 对象转化为字节流，写入到 buffer
	void encode(std::vector<AbstractProtocol::s_ptr>& messages,
	            TcpBuffer::s_ptr out_buffer) override;

	// 将 buffer 里面的字节流转换为 message 对象
	void decode(std::vector<AbstractProtocol::s_ptr>& out_messages,
	            TcpBuffer::s_ptr buffer) override;

	static ProtocolType getProtocalType() {
		return ProtocolType::HTTP_PROTOCOL;
	}

private:
	bool parseHttpRequestLine(const HttpRequest::s_ptr& requset, const std::string& tmp);
	bool parseHttpRequestHeader(const HttpRequest::s_ptr& requset, const std::string& tmp);
	bool parseHttpRequestContent(const HttpRequest::s_ptr& requset, const std::string& tmp);
};

MRPC_NAMESPACE_END

#endif