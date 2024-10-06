#pragma once
#include "CL_VKDevice.h"

namespace Carnival {
	class CL_VKBuffer {
	public:
		CL_VKBuffer(
			CL_VKDevice& device,
			VkDeviceSize instanceSize,
			uint32_t instanceCount,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryFlags,
			VkDeviceSize minOffsetAlignment = 1,
			VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);
		~CL_VKBuffer();

		CL_VKBuffer(const CL_VKBuffer&)				= delete;
		CL_VKBuffer& operator=(const CL_VKBuffer&)	= delete;
		CL_VKBuffer(CL_VKBuffer&&)					= delete;
		CL_VKBuffer& operator=(CL_VKBuffer&&)		= delete;

		VkResult				map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void					unmap();

		void					writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult				flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult				invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkDescriptorBufferInfo	descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

		void					writeToIndex(void* data, int index);
		VkResult				flushIndex(int index);
		VkDescriptorBufferInfo	descriptorInfoForIndex(int index);
		VkResult				invalidateIndex(int index);

		VkBuffer				getBuffer() const				{ return m_Buffer; }
		VkDeviceSize			getBufferSize() const			{ return m_BufferSize; }
		void*					getMappedMemory() const			{ return m_Mapped; }
		uint32_t				getInstanceCount() const		{ return m_InstanceCount; }
		VkDeviceSize			getInstanceSize() const			{ return m_InstanceSize; }
		VkDeviceSize			getAlignmentSize() const		{ return m_AlignmentSize; }
		VkBufferUsageFlags		getUsageFlags() const			{ return m_UsageFlags; }
		VkMemoryPropertyFlags	getMemoryPropertyFlags() const	{ return m_MemoryPropertyFlags; }

	private:
		static VkDeviceSize	getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

		CL_VKDevice&			m_Device;
		void*					m_Mapped = nullptr;
		VkBuffer				m_Buffer = VK_NULL_HANDLE;
		VkDeviceMemory			m_Memory = VK_NULL_HANDLE;

		VkDeviceSize			m_BufferSize;
		uint32_t				m_InstanceCount;
		VkDeviceSize			m_InstanceSize;
		VkDeviceSize			m_AlignmentSize;
		VkBufferUsageFlags		m_UsageFlags;
		VkMemoryPropertyFlags	m_MemoryPropertyFlags;
	};
}