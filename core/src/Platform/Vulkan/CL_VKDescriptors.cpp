#include <clpch.h>
#include "CL_VKDescriptors.h"

namespace Carnival {
//////////////////////////////////////////////////////////////////////////////////////////////////////
// 	   Descriptor Set Layout
//////////////////////////////////////////////////////////////////////////////////////////////////////

	void CL_VKDescriptorSetLayout::addBinding(
		uint32_t			binding,
		VkDescriptorType	type,
		VkShaderStageFlags	stageFlags,
		uint32_t			count)
	{
		CL_CORE_ASSERT(m_Bindings.count(binding) == 0, "Binding Already In Use!");
		VkDescriptorSetLayoutBinding layoutBinding{
			.binding =				binding,
			.descriptorType =		type,
			.descriptorCount =		count,
			.stageFlags =			stageFlags,
			.pImmutableSamplers =	nullptr
		};

		m_Bindings[binding] = layoutBinding;
	}

	void CL_VKDescriptorSetLayout::addBinding(VkDescriptorSetLayoutBinding layoutbinding)
	{
		CL_CORE_ASSERT(m_Bindings.count(layoutbinding.binding) == 0, "Binding Already In Use!");
		m_Bindings[layoutbinding.binding] = layoutbinding;
	}

	void CL_VKDescriptorSetLayout::build(VkDevice device)
	{
		m_Device = device;

		CL_CORE_ASSERT(m_Device != nullptr, "You must pass in Proper Vulkan Device to VKDescriptor Build!");
		CL_CORE_ASSERT(m_Bindings.size() > 0, "There must be bindings added to Vulkan Descriptor Set Layout before Build!");

		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
		for (const auto& pair : m_Bindings) {
			setLayoutBindings.emplace_back(pair.second);
		}

		VkDescriptorSetLayoutCreateInfo info{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(setLayoutBindings.size()),
			.pBindings = setLayoutBindings.data()
		};
		if (vkCreateDescriptorSetLayout(
			m_Device,
			&info,
			nullptr,
			&m_DescriptorSetLayout) != VK_SUCCESS) {
			CL_CORE_CRITICAL("Failed to create Descriptor Set Layouts");
			throw std::runtime_error("Failed to create Descriptor Set Layouts");
		}
	}

	CL_VKDescriptorSetLayout::~CL_VKDescriptorSetLayout()
	{
		if (m_Device) {
			vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);
		}
	}

	std::vector<VkDescriptorPoolSize> CL_VKDescriptorSetLayout::getSizes() const
	{
		std::vector<VkDescriptorPoolSize> Sizes;

		for (const auto& pair : m_Bindings)
		{
			Sizes.emplace_back(pair.second.descriptorType, pair.second.descriptorCount);
		}

		return Sizes;
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 	   Descriptor Pool
//////////////////////////////////////////////////////////////////////////////////////////////////////

	CL_VKDescriptorPoolGrowable::CL_VKDescriptorPoolGrowable(
		CL_VKDevice&								device,
		uint32_t									maxSets,
		VkDescriptorPoolCreateFlags					poolFlags,
		const std::vector<VkDescriptorPoolSize>&	poolSizes)
		: m_Device{ device }, m_MaxSets{ maxSets }, m_PoolFlags{ poolFlags }
	{
		for (const auto& size : poolSizes) m_PoolSizes.emplace_back(size);

		m_AvailablePools.emplace_back(createPool());
		m_MaxSets *= 1.5;
	}

	CL_VKDescriptorPoolGrowable::~CL_VKDescriptorPoolGrowable()
	{
		for (auto pool : m_FullPools)
		{
			vkDestroyDescriptorPool(m_Device.device(), pool, nullptr);
		}
		m_FullPools.clear();

		for (auto pool : m_AvailablePools)
		{
			vkDestroyDescriptorPool(m_Device.device(), pool, nullptr);
		}
		m_AvailablePools.clear();
	}

	VkDescriptorPool CL_VKDescriptorPoolGrowable::createPool()
	{
		VkDescriptorPoolCreateInfo info{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = m_PoolFlags,
			.maxSets = m_MaxSets,
			.poolSizeCount = static_cast<uint32_t>(m_PoolSizes.size()),
			.pPoolSizes = m_PoolSizes.data(),
		};
		VkDescriptorPool pool{};
		if (vkCreateDescriptorPool(m_Device.device(), &info, nullptr, &pool) != VK_SUCCESS) {
			CL_CORE_CRITICAL("Failed To Create Descriptor Pool");
			throw std::runtime_error("Failed To Create Descriptor Pool");
		}
		return pool;
	}

	void CL_VKDescriptorPoolGrowable::allocateDescriptorSet(
		const VkDescriptorSetLayout descriptorSetLayout,
		VkDescriptorSet& descriptorSet)
	{
		VkDescriptorPool pool = getPool();
		VkDescriptorSetAllocateInfo info{
			.sType =				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext =				nullptr,
			.descriptorPool =		pool,
			.descriptorSetCount =	1,
			.pSetLayouts =			&descriptorSetLayout
		};
		VkResult result = vkAllocateDescriptorSets(m_Device.device(), &info, &descriptorSet);

		if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL) {
			m_FullPools.emplace_back(pool);
			
			pool = getPool();
			info.descriptorPool = pool;
			if (vkAllocateDescriptorSets(m_Device.device(), &info, &descriptorSet) != VK_SUCCESS) {
				CL_CORE_CRITICAL("Failed to allocate vulkan descriptor set twice!");
				throw std::runtime_error("Failed to allocate vulkan descriptor set twice!");
			}
		}
		m_AvailablePools.emplace_back(pool);
	}

	// In the following loops Pool is a type-def'd pointer, so no need for auto&
	void CL_VKDescriptorPoolGrowable::resetPools()
	{
		for (auto pool : m_AvailablePools) 
		{
			vkResetDescriptorPool(m_Device.device(), pool, 0);
		}
		for (auto pool : m_FullPools)
		{
			vkResetDescriptorPool(m_Device.device(), pool, 0);
			m_AvailablePools.emplace_back(pool);
		}
		m_FullPools.clear();
	}

	VkDescriptorPool CL_VKDescriptorPoolGrowable::getPool()
	{
		VkDescriptorPool newPool{};
		if (m_AvailablePools.size() != 0)
		{
			newPool = m_AvailablePools.back();
			m_AvailablePools.pop_back();
		}
		else
		{
			newPool = createPool();

			m_MaxSets *= 1.5;
			if (m_MaxSets > 1024)	m_MaxSets = 1024;
		}
		return newPool;
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 	   Descriptor Writer
//////////////////////////////////////////////////////////////////////////////////////////////////////

	CL_VKDescriptorWriter::CL_VKDescriptorWriter(
		CL_VKDescriptorSetLayout&	setLayout,
		CL_VKDescriptorPoolGrowable&		pool)
		: m_SetLayout{ setLayout }, m_Pool{pool}
	{}

	CL_VKDescriptorWriter& CL_VKDescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
	{
		CL_CORE_ASSERT(m_SetLayout.m_Bindings.count(binding) == 1, "Layout Does Not Contain Specified Binding");

		auto& bindingDescription = m_SetLayout.m_Bindings[binding];
		CL_CORE_ASSERT(bindingDescription.descriptorCount == 1, "Binding Single Descripter Info but binding Expects Multiple!");

		m_Writes.emplace_back(
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, // sType
			nullptr, // pNext
			nullptr, // dstSet
			binding, // dstBinding
			0, // dstArrayElement
			1, // descriptorCount
			bindingDescription.descriptorType, // descriptorType
			nullptr, // pImageInfo
			bufferInfo, // pBufferInfo
			nullptr); // pTexelBufferView
		return *this;
	}

	CL_VKDescriptorWriter& CL_VKDescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo)
	{
		CL_CORE_ASSERT(m_SetLayout.m_Bindings.count(binding) == 1, "Layout Does Not Contain Specified Binding");

		auto& bindingDescription = m_SetLayout.m_Bindings[binding];
		CL_CORE_ASSERT(bindingDescription.descriptorCount == 1, "Binding Single Descripter Info but binding Expects Multiple!");

		m_Writes.emplace_back(
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, // sType
			nullptr, // pNext
			nullptr, // dstSet
			binding, // dstBinding
			0, // dstArrayElement
			1, // descriptorCount
			bindingDescription.descriptorType, // descriptorType
			imageInfo, // pImageInfo
			nullptr, // pBufferInfo
			nullptr); // pTexelBufferView

		return *this;
	}

	void CL_VKDescriptorWriter::build(VkDescriptorSet& set)
	{
		m_Pool.allocateDescriptorSet(m_SetLayout.getLayout(), set);
		overwrite(set);
	}

	void CL_VKDescriptorWriter::overwrite(VkDescriptorSet& set)
	{
		for (auto& write : m_Writes) {
			write.dstSet = set;
		}
		vkUpdateDescriptorSets(m_Pool.m_Device.device(), m_Writes.size(), m_Writes.data(), 0, nullptr);
	}
}