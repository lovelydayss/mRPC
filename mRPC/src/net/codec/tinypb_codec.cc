#include "tinypb_codec.h"
#include "tinypb_protocol.h"
#include "utils.h"
#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <vector>

MRPC_NAMESPACE_BEGIN

// 将 message 对象转化为字节流，写入到 buffer
void TinyPBCodec::encode(std::vector<AbstractProtocol::s_ptr>& messages,
                         TcpBuffer::s_ptr out_buffer) {

	for (const auto& message : messages) {
		std::shared_ptr<TinyPBProtocol> msg =
		    std::static_pointer_cast<TinyPBProtocol>(message);
		uint32_t len = 0;
		const char* buf = encodeTinyPB(msg, len);
		if (buf != nullptr && len != 0) {
			out_buffer->writeToBuffer(buf, static_cast<int>(len));
		}
		if (buf) {
			free((void*)buf);
			buf = nullptr;
		}
	}
}

// 将 buffer 里面的字节流转换为 message 对象
void TinyPBCodec::decode(std::vector<AbstractProtocol::s_ptr>& out_messages,
                         TcpBuffer::s_ptr buffer) {
	while (true) {
		// 遍历 buffer，找到
		// PB_START，找到之后，解析出整包的长度。然后得到结束符的位置，判断是否为
		// PB_END
		std::vector<char> tmp = buffer->m_buffer;
		uint32_t start_index = buffer->readIndex();
		uint32_t end_index = -1;

		uint32_t pk_len = 0;
		bool parse_flag = false;
		uint32_t i = 0;

        // 解析起始和结束字符，得到整个包
		for (i = start_index; i < buffer->writeIndex(); ++i) {
			if (tmp[i] == TinyPBProtocol::PB_START) {
				// 读下去四个字节。由于是网络字节序，需要转为主机字节序
				if (i + 1 < buffer->writeIndex()) {
					pk_len = getUInt32FromNetByte(&tmp[i + 1]);
					DEBUGFMTLOG("get pk_len = {}", pk_len);

					// 结束符的索引
					uint32_t j = i + pk_len - 1;
					if (j >= buffer->writeIndex()) {
						continue;
					}
					if (tmp[j] == TinyPBProtocol::PB_END) {
						start_index = i;
						end_index = j;
						parse_flag = true;
						break;
					}
				}
			}
		}

		// 避免未初始化字符干扰
		if (i >= buffer->writeIndex()) {
			DEBUGFMTLOG("decode end, read all buffer data");
			return;
		}

		if (parse_flag == true) {
            
            // 解析整包长度
			buffer->moveReadIndex(end_index - start_index + 1);
			std::shared_ptr<TinyPBProtocol> message =
			    std::make_shared<TinyPBProtocol>();
			message->m_pk_len = pk_len;

            // 解析 msg_id
			uint32_t msg_id_len_index =
			    start_index + sizeof(char) + sizeof(message->m_pk_len);
			if (msg_id_len_index >= end_index) {
				message->parse_flag = false;
				ERRORFMTLOG("parse error, msg_id_len_index[{}] >= end_index[{}]",
				         msg_id_len_index, end_index);
				continue;
			}
			message->m_msg_id_len = getUInt32FromNetByte(&tmp[msg_id_len_index]);
			DEBUGFMTLOG("parse msg_id_len={}", message->m_msg_id_len);

			uint32_t msg_id_index = msg_id_len_index + sizeof(message->m_msg_id_len);

			char msg_id[100] = {0};
			memcpy(&msg_id[0], &tmp[msg_id_index], message->m_msg_id_len);
			message->m_msg_id = std::string(msg_id);
			DEBUGFMTLOG("parse msg_id={}", message->m_msg_id.c_str());

            // 解析方法名
			uint32_t method_name_len_index = msg_id_index + message->m_msg_id_len;
			if (method_name_len_index >= end_index) {
				message->parse_flag = false;
				ERRORFMTLOG(
				    "parse error, method_name_len_index[{}] >= end_index[{}]",
				    method_name_len_index, end_index);
				continue;
			}
			message->m_method_name_len =
			    getUInt32FromNetByte(&tmp[method_name_len_index]);

			uint32_t method_name_index =
			    method_name_len_index + sizeof(message->m_method_name_len);
			char method_name[512] = {0};
			memcpy(&method_name[0], &tmp[method_name_index],
			       message->m_method_name_len);
			message->m_method_name = std::string(method_name);
			DEBUGFMTLOG("parse method_name={}", message->m_method_name.c_str());

            // 解析错误码
			uint32_t err_code_index = method_name_index + message->m_method_name_len;
			if (err_code_index >= end_index) {
				message->parse_flag = false;
				ERRORFMTLOG("parse error, err_code_index[{}] >= end_index[{}]",
				         err_code_index, end_index);
				continue;
			}
			message->m_err_code = getUInt32FromNetByte(&tmp[err_code_index]);

			uint32_t error_info_len_index =
			    err_code_index + sizeof(message->m_err_code);
			if (error_info_len_index >= end_index) {
				message->parse_flag = false;
				ERRORFMTLOG(
				    "parse error, error_info_len_index[{}] >= end_index[{}]",
				    error_info_len_index, end_index);
				continue;
			}
			message->m_err_info_len =
			    getUInt32FromNetByte(&tmp[error_info_len_index]);

			uint32_t err_info_index =
			    error_info_len_index + sizeof(message->m_err_info_len);
			char error_info[512] = {0};
			memcpy(&error_info[0], &tmp[err_info_index],
			       message->m_err_info_len);
			message->m_err_info = std::string(error_info);
			DEBUGFMTLOG("parse error_info={}", message->m_err_info.c_str());

            // 解析 protobuf 数据
			uint32_t pb_data_len = message->m_pk_len - message->m_method_name_len -
			                  message->m_msg_id_len - message->m_err_info_len -
			                  2 - 24;

			uint32_t pd_data_index = err_info_index + message->m_err_info_len;
			message->m_pb_data = std::string(&tmp[pd_data_index], pb_data_len);

			// ***************************************
            //  解析校验和 to do
			message->parse_flag = true;
            // **************************************
            
            // 解析成功，添加消息
			out_messages.push_back(message);
		}
	}
}

const char* TinyPBCodec::encodeTinyPB(const TinyPBProtocol::s_ptr& message,
                                      uint32_t& len) {
	// 填充 msg id
    if (message->m_msg_id.empty()) {
		message->m_msg_id = "123456789";
	}
    
    // 填充整包长度
	DEBUGFMTLOG("msg_id = {}", message->m_msg_id.c_str());
	uint32_t pk_len = 2 + 24 + message->m_msg_id.length() +
	             message->m_method_name.length() +
	             message->m_err_info.length() + message->m_pb_data.length();
	DEBUGFMTLOG("pk_len = {}", pk_len);

	char* buf = reinterpret_cast<char*>(malloc(pk_len));
	char* tmp = buf;

	*tmp = TinyPBProtocol::PB_START;
	tmp++;
    
	uint32_t pk_len_net = htonl(pk_len);
	memcpy(tmp, &pk_len_net, sizeof(pk_len_net));
	tmp += sizeof(pk_len_net);

    // 填充 msg_id
	uint32_t msg_id_len = message->m_msg_id.length();
	uint32_t msg_id_len_net = htonl(msg_id_len);
	memcpy(tmp, &msg_id_len_net, sizeof(msg_id_len_net));
	tmp += sizeof(msg_id_len_net);

	if (!message->m_msg_id.empty()) {
		memcpy(tmp, &(message->m_msg_id[0]), msg_id_len);
		tmp += msg_id_len;
	}
    
    // 填充方法名 method
	uint32_t method_name_len = message->m_method_name.length();
	uint32_t method_name_len_net = htonl(method_name_len);
	memcpy(tmp, &method_name_len_net, sizeof(method_name_len_net));
	tmp += sizeof(method_name_len_net);

	if (!message->m_method_name.empty()) {
		memcpy(tmp, &(message->m_method_name[0]), method_name_len);
		tmp += method_name_len;
	}

    // 填充错误码
	uint32_t err_code_net = htonl(message->m_err_code);
	memcpy(tmp, &err_code_net, sizeof(err_code_net));
	tmp += sizeof(err_code_net);

	uint32_t err_info_len = message->m_err_info.length();
	uint32_t err_info_len_net = htonl(err_info_len);
	memcpy(tmp, &err_info_len_net, sizeof(err_info_len_net));
	tmp += sizeof(err_info_len_net);

	if (!message->m_err_info.empty()) {
		memcpy(tmp, &(message->m_err_info[0]), err_info_len);
		tmp += err_info_len;
	}

	if (!message->m_pb_data.empty()) {
		memcpy(tmp, &(message->m_pb_data[0]), message->m_pb_data.length());
		tmp += message->m_pb_data.length();
	}

	uint32_t check_sum_net = htonl(1);
	memcpy(tmp, &check_sum_net, sizeof(check_sum_net));
	tmp += sizeof(check_sum_net);

	*tmp = TinyPBProtocol::PB_END;

	message->m_pk_len = pk_len;
	message->m_msg_id_len = msg_id_len;
	message->m_method_name_len = method_name_len;
	message->m_err_info_len = err_info_len;
	message->parse_flag = true;
	len = pk_len;

	DEBUGFMTLOG("encode message[{}] success", message->m_msg_id.c_str());

	return buf;
}

MRPC_NAMESPACE_END