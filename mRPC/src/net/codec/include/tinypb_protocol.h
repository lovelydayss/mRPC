#ifndef MRPC_NET_CODEC_TINYPB_PROTOCOL_H
#define MRPC_NET_CODEC_TINYPB_PROTOCOL_H

#include "abstract_protocol.h"
#include "utils.h"
#include <memory>
#include <string>

MRPC_NAMESPACE_BEGIN

class TinyPBProtocol : public AbstractProtocol {
public:
	using s_ptr = std::shared_ptr<TinyPBProtocol>;

public:
	TinyPBProtocol() = default;
	~TinyPBProtocol() = default;

public:
	static char PB_START;           	// 开始符
	static char PB_END;             	// 结束符

public:
	uint32_t m_pk_len{0};            	// 整包长度
	uint32_t m_msg_id_len{0};        	// msg_id 长度
	/* msg_id 继承父类  */ 			 

	uint32_t m_method_name_len{0};   	// 调用方法名称长度
	std::string m_method_name;      	// 调用方法名称

	uint32_t m_err_code{0};          	// 错误码
	uint32_t m_err_info_len{0};      	// 错误信息长度
	std::string m_err_info;         	// 错误信息

	std::string m_pb_data;          	// protobuf 序列化数据
	uint32_t m_check_sum{0};         	// CRC 校验码

	bool parse_flag{false};         	// 解析标志位(true->解析成功 false->解析失败)
};

MRPC_NAMESPACE_END

#endif