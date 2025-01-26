#include <clpch.h>
#include "VKBuffer.h"

namespace Carnival {
	CL_VKBuffer::CL_VKBuffer(
		CL_VKDevice& device,
		VkDeviceSize instanceSize,
		uint32_t instanceCount,
		VkBufferUsageFlags usageFlags,
		VkMemoryPropertyFlags memoryFlags,
		VkDeviceSize minOffsetAlignment,
		VkSharingMode sharingMode)
		:	m_Device{device},
			m_InstanceSize{ instanceSize },
			m_InstanceCount{ instanceCount },
			m_UsageFlags{ usageFlags },
			m_MemoryPropertyFlags{ memoryFlags }
	{
		m_AlignmentSize = getAlignment(m_InstanceSize, minOffsetAlignment);
		m_BufferSize = m_AlignmentSize * m_InstanceCount;
		m_Device.createBuffer(m_BufferSize, m_UsageFlags, m_MemoryPropertyFlags, m_Buffer, m_Memory, sharingMode);
	}
	CL_VKBuffer::~CL_VKBuffer()
	{
		unmap();
		vkDestroyBuffer(m_Device.device(), m_Buffer, nullptr);
		vkFreeMemory(m_Device.device(), m_Memory, nullptr);
	}

	VkResult CL_VKBuffer::map(VkDeviceSize size, VkDeviceSize offset)
	{
		CL_CORE_ASSERT((m_Buffer && m_Memory), "Vulkan: Called Map on buffer before Creation");
		return vkMapMemory(m_Device.device(), m_Memory, offset, size, 0, &m_Mapped);
	}
	void CL_VKBuffer::unmap()
	{
		if (m_Mapped) {
			vkUnmapMemory(m_Device.device(), m_Memory);
			m_Mapped = nullptr;
		}
	}

	void CL_VKBuffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
	{
		CL_CORE_ASSERT(m_Mapped, "Vulkan: Cannot Write to Unmapped Buffer");

		if (size == VK_WHOLE_SIZE)	memcpy(m_Mapped, data, m_BufferSize);
		else {
			char* memOffset = (char*)m_Mapped;
			memOffset += offset;
			memcpy(memOffset, data, size);
		}
	}
	// TODO: Future optimization is to flush multiple mapped ranges at once,
	// call to multiple buffers async? multiple maps on a single buffer possible as well
	VkResult CL_VKBuffer::flush(VkDeviceSize size, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange{
			.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			.pNext = nullptr,
			.memory = m_Memory,
			.offset = offset,
			.size = size
		};
		return vkFlushMappedMemoryRanges(m_Device.device(), 1, &mappedRange);
	}
	VkResult CL_VKBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange{
			.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			.pNext = nullptr,
			.memory = m_Memory,
			.offset = offset,
			.size = size
		};
		return vkInvalidateMappedMemoryRanges(m_Device.device(), 1, &mappedRange);
	}
	VkDescriptorBufferInfo CL_VKBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset)
	{
		return VkDescriptorBufferInfo{
			m_Buffer,
			offset,
			size
		};
	}

	// Could also write padding as 0?
	void CL_VKBuffer::writeToIndex(void* data, int index)					{ writeToBuffer(data, m_InstanceSize, index * m_AlignmentSize); }
	VkResult CL_VKBuffer::flushIndex(int index)								{ return flush(m_AlignmentSize, index * m_AlignmentSize); }
	VkDescriptorBufferInfo CL_VKBuffer::descriptorInfoForIndex(int index)	{ return descriptorInfo(m_AlignmentSize, index * m_AlignmentSize); }
	VkResult CL_VKBuffer::invalidateIndex(int index)						{ return invalidate(m_AlignmentSize, index * m_AlignmentSize); }

	VkDeviceSize CL_VKBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
	{
		if (minOffsetAlignment > 0) {
			return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
		}
		return instanceSize;
	}
}
