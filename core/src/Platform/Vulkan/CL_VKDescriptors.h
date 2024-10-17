#pragma once
#include "CL_VKDevice.h"

// I hate Everything in this file

namespace Carnival {
//////////////////////////////////////////////////////////////////////////////////////////////////////
// 	   DescriptorSetLayout
//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Removed Builder SubClass, Less allocations and copies at the cost of imperfect RAII
	class CL_VKDescriptorSetLayout {
	public:
		CL_VKDescriptorSetLayout() {}
		void addBinding(
			uint32_t			binding,
			VkDescriptorType	type,
			VkShaderStageFlags	stageFlags,
			uint32_t			count = 1);
		void addBinding(VkDescriptorSetLayoutBinding layoutbinding);

		void build(VkDevice device);

		~CL_VKDescriptorSetLayout();

		CL_VKDescriptorSetLayout(const CL_VKDescriptorSetLayout&)				= delete;
		CL_VKDescriptorSetLayout& operator=(const CL_VKDescriptorSetLayout&)	= delete;

		// Returns a TypeDef'd Pointer
		VkDescriptorSetLayout				getLayout() const { return m_DescriptorSetLayout; }
		std::vector<VkDescriptorPoolSize>	getSizes() const;
	private:
		VkDevice													m_Device = nullptr;
		VkDescriptorSetLayout										m_DescriptorSetLayout{};
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>	m_Bindings{};

		friend class CL_VKDescriptorWriter;
	};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 	   DescriptorPool
//////////////////////////////////////////////////////////////////////////////////////////////////////

	class CL_VKDescriptorPoolGrowable {
	public:
		CL_VKDescriptorPoolGrowable(
			CL_VKDevice&								device,
			uint32_t									maxSets,
			VkDescriptorPoolCreateFlags					poolFlags,
			const std::vector<VkDescriptorPoolSize>&	poolSizes);
		~CL_VKDescriptorPoolGrowable();

		CL_VKDescriptorPoolGrowable(const CL_VKDescriptorPoolGrowable&)				= delete;
		CL_VKDescriptorPoolGrowable& operator=(const CL_VKDescriptorPoolGrowable&)	= delete;

		void allocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor);

		void resetPools();
	private:
		VkDescriptorPool getPool();
		VkDescriptorPool createPool();

		CL_VKDevice&						m_Device;
		std::vector<VkDescriptorPool>		m_AvailablePools;
		std::vector<VkDescriptorPool>		m_FullPools;
		std::vector<VkDescriptorPoolSize>	m_PoolSizes;
		
		uint32_t							m_MaxSets;
		VkDescriptorPoolCreateFlags			m_PoolFlags;

		friend class CL_VKDescriptorWriter;
	};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 	   DescriptorWriter
//////////////////////////////////////////////////////////////////////////////////////////////////////

	class CL_VKDescriptorWriter {
	public:
		CL_VKDescriptorWriter(CL_VKDescriptorSetLayout& setLayout, CL_VKDescriptorPoolGrowable& pool);

		CL_VKDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		CL_VKDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		void build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);
	private:
		CL_VKDescriptorSetLayout&			m_SetLayout;
		CL_VKDescriptorPoolGrowable&		m_Pool;
		std::vector<VkWriteDescriptorSet>	m_Writes;
	};
}