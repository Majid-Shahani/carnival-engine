#include <clpch.h>
#include "CL_VKDescriptors.h"

namespace Carnival {
//////////////////////////////////////////////////////////////////////////////////////////////////////
// 	   Descriptor Set Layout Builder
//////////////////////////////////////////////////////////////////////////////////////////////////////

	CL_VKDescriptorSetLayout::Builder& CL_VKDescriptorSetLayout::Builder::addBinding(
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
		return *this;
	}

	CL_VKDescriptorSetLayout::Builder& CL_VKDescriptorSetLayout::Builder::addBinding(VkDescriptorSetLayoutBinding layoutbinding)
	{
		CL_CORE_ASSERT(m_Bindings.count(layoutbinding.binding) == 0, "Binding Already In Use!");
		m_Bindings[layoutbinding.binding] = layoutbinding;
		return *this;
	}

	std::unique_ptr<CL_VKDescriptorSetLayout> CL_VKDescriptorSetLayout::Builder::build() const
	{
		return std::make_unique<CL_VKDescriptorSetLayout>(m_Device, m_Bindings);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 	   Descriptor Set Layout
//////////////////////////////////////////////////////////////////////////////////////////////////////

	CL_VKDescriptorSetLayout::CL_VKDescriptorSetLayout(
		CL_VKDevice& device,
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
		: m_Device(device), m_Bindings {bindings}
	{
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
		for (const auto& pair : bindings) {
			setLayoutBindings.push_back(pair.second);
		}

		VkDescriptorSetLayoutCreateInfo info{
			.sType =		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext =		nullptr,
			.flags =		0,
			.bindingCount =	static_cast<uint32_t>(setLayoutBindings.size()),
			.pBindings =	setLayoutBindings.data()
		};
		if (vkCreateDescriptorSetLayout(
			m_Device.device(),
			&info,
			nullptr,
			&m_DescriptorSetLayout) != VK_SUCCESS) {
				CL_CORE_CRITICAL("Failed to create Descriptor Set Layouts");
				throw std::runtime_error("Failed to create Descriptor Set Layouts");
		}
	}

	CL_VKDescriptorSetLayout::~CL_VKDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(m_Device.device(), m_DescriptorSetLayout, nullptr);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 	   Descriptor Pool Builder
//////////////////////////////////////////////////////////////////////////////////////////////////////

	CL_VKDescriptorPool::Builder& CL_VKDescriptorPool::Builder::addPoolSizes(VkDescriptorType type, uint32_t count)
	{
		m_PoolSizes.push_back({type, count});
		return *this;
	}
	CL_VKDescriptorPool::Builder& CL_VKDescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags)
	{
		m_PoolFlags = flags;
		return *this;
	}
	CL_VKDescriptorPool::Builder& CL_VKDescriptorPool::Builder::setMaxSets(uint32_t count)
	{
		m_MaxSets = count;
		return *this;
	}
	std::unique_ptr<CL_VKDescriptorPool> CL_VKDescriptorPool::Builder::build() const
	{
		return std::make_unique<CL_VKDescriptorPool>(m_Device, m_MaxSets, m_PoolFlags, m_PoolSizes);
	}

	CL_VKDescriptorPool::CL_VKDescriptorPool(
		CL_VKDevice&								device,
		uint32_t									maxSets,
		VkDescriptorPoolCreateFlags					poolFlags,
		const std::vector<VkDescriptorPoolSize>&	poolSizes)
		: m_Device{device}
	{
		VkDescriptorPoolCreateInfo info{
			.sType =			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.pNext =			nullptr,
			.flags =			poolFlags,
			.maxSets =			maxSets,
			.poolSizeCount =	static_cast<uint32_t>(poolSizes.size()),
			.pPoolSizes =		poolSizes.data(),
		};
		if (vkCreateDescriptorPool(m_Device.device(), &info, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
			CL_CORE_CRITICAL("Failed To Create Descriptor Pool");
			throw std::runtime_error("Failed To Create Descriptor Pool");
		}
		// TODO: Add to Available Pools Array
	}
	CL_VKDescriptorPool::~CL_VKDescriptorPool()
	{
		vkDestroyDescriptorPool(m_Device.device(), m_DescriptorPool, nullptr);
	}

	bool CL_VKDescriptorPool::allocateDescriptorSet(
		const VkDescriptorSetLayout descriptorSetLayout,
		VkDescriptorSet& descriptor) const
	{
		VkDescriptorSetAllocateInfo info{
			.sType =				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext =				nullptr,
			.descriptorPool =		m_DescriptorPool,
			.descriptorSetCount =	1,
			.pSetLayouts =			&descriptorSetLayout
		};
		if (vkAllocateDescriptorSets(m_Device.device(), &info, &descriptor) != VK_SUCCESS) {
			// TODO: Add pool to full pools
			return false;
		}
		return true;
	}
	void CL_VKDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
	{
		vkFreeDescriptorSets(
			m_Device.device(),
			m_DescriptorPool,
			static_cast<uint32_t>(descriptors.size()),
			descriptors.data());
	}
	void CL_VKDescriptorPool::resetPool()
	{
		vkResetDescriptorPool(m_Device.device(), m_DescriptorPool, 0);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 	   Descriptor Writer
//////////////////////////////////////////////////////////////////////////////////////////////////////

	CL_VKDescriptorWriter::CL_VKDescriptorWriter(
		CL_VKDescriptorSetLayout&	setLayout,
		CL_VKDescriptorPool&		pool)
		: m_SetLayout{ setLayout }, m_Pool{pool}
	{}

	CL_VKDescriptorWriter& CL_VKDescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
	{
		CL_CORE_ASSERT(m_SetLayout.m_Bindings.count(binding) == 1, "Layout Does Not Contain Specified Binding");

		auto& bindingDescription = m_SetLayout.m_Bindings[binding];
		CL_CORE_ASSERT(bindingDescription.descriptorCount == 1, "Binding Single Descripter Info but binding Expects Multiple!");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorCount = 1;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;

		m_Writes.push_back(write);
		return *this;
	}

	CL_VKDescriptorWriter& CL_VKDescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo)
	{
		CL_CORE_ASSERT(m_SetLayout.m_Bindings.count(binding) == 1, "Layout Does Not Contain Specified Binding");

		auto& bindingDescription = m_SetLayout.m_Bindings[binding];
		CL_CORE_ASSERT(bindingDescription.descriptorCount == 1, "Binding Single Descripter Info but binding Expects Multiple!");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorCount = 1;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = imageInfo;

		m_Writes.push_back(write);
		return *this;
	}

	bool CL_VKDescriptorWriter::build(VkDescriptorSet& set)
	{
		bool success = m_Pool.allocateDescriptorSet(m_SetLayout.getLayout(), set);
		if (!success)	return false;
		overwrite(set);
		return true;
	}

	void CL_VKDescriptorWriter::overwrite(VkDescriptorSet& set)
	{
		for (auto& write : m_Writes) {
			write.dstSet = set;
		}
		vkUpdateDescriptorSets(m_Pool.m_Device.device(), m_Writes.size(), m_Writes.data(), 0, nullptr);
	}
}