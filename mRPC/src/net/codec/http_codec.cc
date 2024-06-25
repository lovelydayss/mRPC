#include "http_codec.h"
#include "http_protocol.h"
#include "utils.h"

MRPC_NAMESPACE_BEGIN

void HTTPCodec::encode(std::vector<AbstractProtocol::s_ptr>& messages,
                       TcpBuffer::s_ptr out_buffer) {}

void HTTPCodec::decode(std::vector<AbstractProtocol::s_ptr>& out_messages,
                       TcpBuffer::s_ptr buffer) {}

bool HTTPCodec::parseHttpRequestLine(const HttpRequest::s_ptr& requset,/* NOLINT */
                                     const std::string& tmp) {
	return true;
}

bool HTTPCodec::parseHttpRequestHeader(const HttpRequest::s_ptr& requset,/* NOLINT */
                                       const std::string& tmp) { 
	return true;
}

bool HTTPCodec::parseHttpRequestContent(const HttpRequest::s_ptr& requset,/* NOLINT */
                                        const std::string& tmp) { 
	return true;
}

MRPC_NAMESPACE_END