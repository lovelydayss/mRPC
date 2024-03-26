#include "http_codec.h"
#include "http_protocol.h"
#include "utils.h"

MRPC_NAMESPACE_BEGIN

	void HTTPCodec::encode(std::vector<AbstractProtocol::s_ptr>& messages,
	            TcpBuffer::s_ptr out_buffer) {

                }

	void HTTPCodec::decode(std::vector<AbstractProtocol::s_ptr>& out_messages,
	            TcpBuffer::s_ptr buffer) {

                }

	bool HTTPCodec::parseHttpRequestLine(const HttpRequest::s_ptr& requset, const std::string& tmp) {
        
    }

	bool HTTPCodec::parseHttpRequestHeader(const HttpRequest::s_ptr& requset, const std::string& tmp) {

    }

	bool HTTPCodec::parseHttpRequestContent(const HttpRequest::s_ptr& requset, const std::string& tmp) {

    }




MRPC_NAMESPACE_END