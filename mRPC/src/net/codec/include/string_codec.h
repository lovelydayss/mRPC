#ifndef MRPC_NET_CODEC_STRING_CODER_H
#define MRPC_NET_CODEC_STRING_CODER_H

#include "abstract_codec.h"
#include "abstract_protocol.h"
#include "utils.h"
#include <memory>

MRPC_NAMESPACE_BEGIN

// 协议定义
class StringProtocol : public AbstractProtocol {
public:
	using s_ptr = std::shared_ptr<StringProtocol>;

public:
	std::string info; // 字符串信息
};

class StringCoder : public AbstractCodec {
	// 将 message 对象转化为字节流，写入到 buffer
	void encode(std::vector<AbstractProtocol::s_ptr>& messages,
	            TcpBuffer::s_ptr out_buffer) override {

		for (const auto& message : messages) {
			StringProtocol::s_ptr msg =
			    std::static_pointer_cast<StringProtocol>(message);
			out_buffer->writeToBuffer(msg->info.c_str(),
			                          static_cast<int>(msg->info.length()));
		}
	}

	// 将 buffer 里面的字节流转换为 message 对象
	void decode(std::vector<AbstractProtocol::s_ptr>& out_messages,
	            TcpBuffer::s_ptr buffer) override {

		std::vector<char> re;
		buffer->readFromBuffer(re, buffer->readAble());
		std::string info(re.begin(), re.end());

		StringProtocol::s_ptr msg = std::make_shared<StringProtocol>();
		msg->info = info;
		msg->m_msg_id = "123456";
		out_messages.push_back(msg);
	}
};

MRPC_NAMESPACE_END

#endif