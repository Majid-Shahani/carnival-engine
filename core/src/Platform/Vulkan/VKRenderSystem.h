#pragma once

#include "VKDevice.h"
#include "VKPipeline.h"
#include "VKModel.h"

namespace Carnival {
	class CL_VKRenderSystem {
	public:
		CL_VKRenderSystem(CL_VKDevice& device, VkRenderPass renderPass, VkExtent2D extent, VkDescriptorSetLayout setLayout);
		~CL_VKRenderSystem();

		CL_VKRenderSystem(const CL_VKRenderSystem&) = delete;
		CL_VKRenderSystem& operator=(const CL_VKRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandbuffer, std::vector<CL_VKModel>& models, VkDescriptorSet ds);

	private:
		void createPipelineLayout(VkDescriptorSetLayout setLayout);
		void createPipeline(VkRenderPass renderPass, VkExtent2D extent);

		CL_VKDevice& m_Device;
		std::unique_ptr<CL_VKPipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}