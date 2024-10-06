#pragma once
#include "CL_VKDevice.h"

// I hate Everything in this file

namespace Carnival {
//////////////////////////////////////////////////////////////////////////////////////////////////////
// 	   DescriptorSetLayout
//////////////////////////////////////////////////////////////////////////////////////////////////////

	class CL_VKDescriptorSetLayout {
	public:
		class Builder { // perhaps objects of this type should be local and go out of scope / be destructed
		public:
			Builder(CL_VKDevice& device) : m_Device{ device } {}

			Builder& addBinding(
				uint32_t			binding,
				VkDescriptorType	type,
				VkShaderStageFlags	stageFlags,
				uint32_t			count = 1);
			Builder& addBinding(VkDescriptorSetLayoutBinding layoutbinding);

			std::unique_ptr<CL_VKDescriptorSetLayout> build() const;

		private:
			CL_VKDevice& m_Device;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings;
		};

		CL_VKDescriptorSetLayout(
			CL_VKDevice&												device,
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>	bindings);
		~CL_VKDescriptorSetLayout();

		CL_VKDescriptorSetLayout(const CL_VKDescriptorSetLayout&)				= delete;
		CL_VKDescriptorSetLayout& operator=(const CL_VKDescriptorSetLayout&)	= delete;

		VkDescriptorSetLayout getLayout() const { return m_DescriptorSetLayout; }
	private:
		CL_VKDevice&												m_Device;
		VkDescriptorSetLayout										m_DescriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>	m_Bindings;

		friend class CL_VKDescriptorWriter;
	};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 	   DescriptorPool
//////////////////////////////////////////////////////////////////////////////////////////////////////

	class CL_VKDescriptorPool {
	public:
		class Builder {
		public:
			Builder(CL_VKDevice& device) : m_Device{ device } {}

			Builder& addPoolSizes(VkDescriptorType type, uint32_t count);
			Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& setMaxSets(uint32_t count);
			std::unique_ptr<CL_VKDescriptorPool> build() const; // Weird as hell returning a unique pointer
		private:
			CL_VKDevice& m_Device;
			std::vector<VkDescriptorPoolSize> m_PoolSizes{};
			uint32_t m_MaxSets = 1000;
			VkDescriptorPoolCreateFlags m_PoolFlags = 0;
		};

		CL_VKDescriptorPool(
			CL_VKDevice&								device,
			uint32_t									maxSets,
			VkDescriptorPoolCreateFlags					poolFlags,
			const std::vector<VkDescriptorPoolSize>&	poolSizes);
		~CL_VKDescriptorPool();

		CL_VKDescriptorPool(const CL_VKDescriptorPool&)				= delete;
		CL_VKDescriptorPool& operator=(const CL_VKDescriptorPool&)	= delete;

		bool allocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;
		void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

		void resetPool();

	private:
		CL_VKDevice&		m_Device;
		VkDescriptorPool	m_DescriptorPool;

		friend class CL_VKDescriptorWriter;
	};

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 	   DescriptorWriter
//////////////////////////////////////////////////////////////////////////////////////////////////////

	class CL_VKDescriptorWriter {
	public:
		CL_VKDescriptorWriter(CL_VKDescriptorSetLayout& setLayout, CL_VKDescriptorPool& pool);

		CL_VKDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		CL_VKDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);
	private:
		CL_VKDescriptorSetLayout&			m_SetLayout;
		CL_VKDescriptorPool&				m_Pool;
		std::vector<VkWriteDescriptorSet>	m_Writes;
	};
}