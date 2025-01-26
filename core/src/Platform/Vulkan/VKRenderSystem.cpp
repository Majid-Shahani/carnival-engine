#include <clpch.h>
#include "VKRenderSystem.h"

namespace Carnival {
	CL_VKRenderSystem::CL_VKRenderSystem(
		CL_VKDevice& device,
		VkRenderPass renderPass,
		VkExtent2D extent,
		VkDescriptorSetLayout setLayout) 
		: m_Device{ device }
	{
		createPipelineLayout(setLayout);
		createPipeline(renderPass, extent);
	}
	CL_VKRenderSystem::~CL_VKRenderSystem()
	{
		vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
	}


	void CL_VKRenderSystem::createPipelineLayout(VkDescriptorSetLayout setLayout)
	{
		// Push Constants
		VkPushConstantRange pcr{
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			.offset = 0,
			.size = sizeof(PushConstantData)
		};

		std::vector<VkDescriptorSetLayout> setLayouts{ setLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = static_cast<uint32_t>(setLayouts.size()),
			.pSetLayouts = setLayouts.data(),
			.pushConstantRangeCount = 1, // Optional
			.pPushConstantRanges = &pcr// Optional
		};

		if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
			CL_CORE_CRITICAL("pipeline layout!");
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
	void CL_VKRenderSystem::createPipeline(VkRenderPass renderPass, VkExtent2D extent)
	{
		CL_CORE_ASSERT(m_PipelineLayout, "Cannot create Pipeline before Pipeline Layout!");
		PipelineConfigInfo configInfo(extent.width, extent.height);
		configInfo.renderPass = renderPass;
		configInfo.pipelineLayout = m_PipelineLayout;

		std::filesystem::path vertPath("./src/shaders/shader.vert.spv");
		std::filesystem::path fragPath("./src/shaders/shader.frag.spv");

		m_Pipeline = std::make_unique<CL_VKPipeline>(
			m_Device,
			vertPath,
			fragPath,
			configInfo);
	}

	void CL_VKRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<CL_VKModel>& models, VkDescriptorSet ds)
	{
		m_Pipeline->bind(commandBuffer);

		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_PipelineLayout,
			0, 1, // first descriptor set to be bound, and the number of sets to be bound
			&ds,
			0, nullptr); // dynamic offsets

		for (auto& model : models) {
			model.bind(commandBuffer);
			for (int j = 0; j < 1; j++) {
				PushConstantData push{
					.offset = {0.0f, j * 0.35f},
					.color = {0.1f * j * j, 0.3f * j, 0.2f + 0.2f * j}
				};

				vkCmdPushConstants(
					commandBuffer,
					m_PipelineLayout,
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
					0,
					sizeof(PushConstantData),
					&push
				);

				model.draw(commandBuffer);
			}
		}
	}
}