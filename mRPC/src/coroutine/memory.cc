#include "memory.h"
#include "log.h"
#include "utils.h"
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <mutex>

MRPC_NAMESPACE_BEGIN

Memory::Memory(uint32_t block_size, uint32_t block_count)
    : m_block_size(block_size)
    , m_block_count(block_count) {

	m_size = m_block_count * m_block_size;
	m_start = make_unique<uint8_t>(static_cast<uint8_t*>(malloc(m_size)));
	INFOLOG("succ mmap %d bytes memory", m_size);

	m_blocks.resize(m_block_count);
	for (auto&& m_block : m_blocks)
		m_block = false;

	m_ref_counts = 0;
}

uint8_t* Memory::getBlock() {
	int t = -1;

	{
		std::lock_guard<std::mutex> lk(m_mutex);
		for (int i = 0; i < m_blocks.size(); ++i) {
			if (m_blocks[i] == false) {
				m_blocks[i] = true;
				t = i;
				break;
			}
		}
	}

	if (t == -1)
		return nullptr;

	m_ref_counts++;
	return m_start.get() + (t * m_block_size);
}

void Memory::backBlock(const uint8_t* addr) {
	if (addr > getEnd() || addr < getStart()) {
        INFOLOG("error, this block is not belong to this Memory%s", "");
		return;
	}

	uint32_t index = (addr - getStart()) / m_block_size;
    {
        std::lock_guard<std::mutex> lk(m_mutex);
	    m_blocks[index] = false;
    }

	m_ref_counts--;
}

bool Memory::hasBlock(const uint8_t* addr) { return ((addr >= getStart()) && (addr <= getEnd())); }


MRPC_NAMESPACE_END
