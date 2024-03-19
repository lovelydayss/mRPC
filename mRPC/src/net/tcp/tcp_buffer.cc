#include "tcp_buffer.h"
#include "log.h"
#include "utils.h"
#include <cstring>

MRPC_NAMESPACE_BEGIN

// 返回可读字节数
int TcpBuffer::readAble() const { return m_write_index - m_read_index; }

// 返回可写的字节数
int TcpBuffer::writeAble() const {
	return static_cast<int>(m_buffer.size()) - m_write_index;
}

int TcpBuffer::getBufferSize() const {
	return static_cast<int>(m_buffer.size());
}

int TcpBuffer::readIndex() const { return m_read_index; }

int TcpBuffer::writeIndex() const { return m_write_index; }

void TcpBuffer::writeToBuffer(const char* buf, int size) {

	if (size > writeAble()) {
		int new_size = static_cast<int>(1.5 * (m_write_index + size));
		resizeBuffer(new_size);
	}

	memcpy(&m_buffer[m_write_index], buf, size);
	m_write_index += size;
}

void TcpBuffer::readFromBuffer(std::vector<char>& re, int size) {
	if (readAble() == 0)
		return;

	int read_size = readAble() > size ? size : readAble();

	std::vector<char> tmp(read_size);
	memcpy(&tmp[0], &m_buffer[m_read_index], read_size);

	re.swap(tmp);
	m_read_index += read_size;

	adjustBuffer();
}

void TcpBuffer::resizeBuffer(int new_size) {
	std::vector<char> tmp(new_size);
	int count = std::min(new_size, readAble());

	memcpy(&tmp[0], &m_buffer[m_read_index], count);
	m_buffer.swap(tmp);

	m_read_index = 0;
	m_write_index = m_read_index + count;
}
void TcpBuffer::adjustBuffer() {
	if (m_read_index < static_cast<int>(m_buffer.size() / 3))
		return;

	std::vector<char> buffer(m_buffer.size());
	int count = readAble();

	memcpy(&buffer[0], &m_buffer[m_read_index], count);
	m_buffer.swap(buffer);
	m_read_index = 0;
	m_write_index = m_read_index + count;

	buffer.clear();
}
void TcpBuffer::moveReadIndex(int size) {

	size_t j = m_read_index + size;
	if (j >= m_buffer.size()) {
		ERRORLOG("moveWriteIndex error, invalid size %d, old_read_index %d, "
		         "buffer size %d",
		         size, m_read_index, m_buffer.size());
		return;
	}
	m_read_index = static_cast<int>(j);
	adjustBuffer();
}
void TcpBuffer::moveWriteIndex(int size) {

	size_t j = m_write_index + size;
	if (j >= m_buffer.size()) {
		ERRORLOG("moveWriteIndex error, invalid size %d, old_read_index %d, "
		         "buffer size %d",
		         size, m_read_index, m_buffer.size());
		return;
	}
	m_write_index = static_cast<int>(j);
	adjustBuffer();
}

MRPC_NAMESPACE_END