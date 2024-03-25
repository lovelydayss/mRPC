#ifndef MRPC_NET_TCP_TCP_BUFFER_H
#define MRPC_NET_TCP_TCP_BUFFER_H

#include "utils.h"
#include <cstddef>
#include <vector>

MRPC_NAMESPACE_BEGIN

// 环形缓冲区实现
class TcpBuffer {
public:
	using s_ptr = std::shared_ptr<TcpBuffer>;

public:
	explicit TcpBuffer(int size) { m_buffer.resize(size); }

	~TcpBuffer() = default;

	// 返回可读字节数
	int readAble() const;

	// 返回可写的字节数
	int writeAble() const;
	int getBufferSize() const;

	int readIndex() const;
	int writeIndex() const;

	void writeToBuffer(const char* buf, int size);
	void readFromBuffer(std::vector<char>& re, int size);

	void resizeBuffer(int new_size);
	void adjustBuffer();
	void moveReadIndex(int size);
	void moveWriteIndex(int size);

private:
	int m_read_index{0};  // 可读位置
	int m_write_index{0}; // 可写位置

public:
	std::vector<char> m_buffer;
};

MRPC_NAMESPACE_END

#endif
