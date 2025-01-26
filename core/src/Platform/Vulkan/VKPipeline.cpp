#include <clpch.h>
#include "VKPipeline.h"
#include "VKModel.h"

namespace Carnival {

	CL_VKPipeline::CL_VKPipeline(
		CL_VKDevice& device,
		const std::filesystem::path& vertShaderPath,
		const std::filesystem::path& fragShaderPath,
		const PipelineConfigInfo& configInfo) : m_CLVKDevice{ device }
	{
		//createGraphicsPipeline(vertShaderPath, fragShaderPath, configInfo);

		CL_CORE_ASSERT(configInfo.pipelineLayout, "Must Provide PipelineLayout in configInfo to createGraphicsPipeline");
		CL_CORE_ASSERT(configInfo.renderPass, "Must Provide RenderPass in Config Info to createGraphicsPipeline");

		// Shader Modules
		auto vertShaderCode = Carnival::readFileCharVec(vertShaderPath);
		auto fragShaderCode = Carnival::readFileCharVec(fragShaderPath);

		VkShaderModule m_VertShaderModule{ VK_NULL_HANDLE }, m_FragShaderModule{ VK_NULL_HANDLE };
		createShaderModule(vertShaderCode, &m_VertShaderModule);
		createShaderModule(fragShaderCode, &m_FragShaderModule);
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = m_VertShaderModule,
			.pName = "main", // Entry Point
			.pSpecializationInfo = nullptr
		};
		VkPipelineShaderStageCreateInfo fragShaderStageInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = m_FragShaderModule,
			.pName = "main",
			.pSpecializationInfo = nullptr
		};
		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		// Vertex Input State
		auto bindingDescription = CL_VKModel::getBindingDescription();
		auto attributeDescriptions = CL_VKModel::getAttributeDescription();
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = 1,
			.pVertexBindingDescriptions = &bindingDescription,
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
			.pVertexAttributeDescriptions = attributeDescriptions.data()
		};

		// Viewport State
		VkPipelineViewportStateCreateInfo viewportState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.viewportCount = 1,
			.pViewports = &configInfo.viewport,
			.scissorCount = 1,
			.pScissors = &configInfo.scissor,
		};

		// Dynamic States
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicState = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
			.pDynamicStates = dynamicStates.data(),
		};

		// Graphics Pipeline
		VkGraphicsPipelineCreateInfo pipelineInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.stageCount = 2,
			.pStages = shaderStages,
			.pVertexInputState = &vertexInputInfo,
			.pInputAssemblyState = &configInfo.inputAssemblyInfo,
			.pViewportState = &viewportState,
			.pRasterizationState = &configInfo.rasterizer,
			.pMultisampleState = &configInfo.multisampling,
			.pDepthStencilState = &configInfo.depthStencil,
			.pColorBlendState = &configInfo.colorBlending,
			.pDynamicState = &dynamicState,
			.layout = configInfo.pipelineLayout,
			.renderPass = configInfo.renderPass,
			.subpass = configInfo.subpass,
			.basePipelineHandle = VK_NULL_HANDLE, // Optional
			.basePipelineIndex = -1, // Optional
		};

		if (vkCreateGraphicsPipelines(m_CLVKDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &m_GraphicsPipeline) != VK_SUCCESS) {
			CL_CORE_CRITICAL("Vulkan Graphics Pipeline Creation Failed!");
			throw std::runtime_error("Failed to create graphics pipeline");
		}

		vkDestroyShaderModule(m_CLVKDevice.device(), m_FragShaderModule, nullptr);
		vkDestroyShaderModule(m_CLVKDevice.device(), m_VertShaderModule, nullptr);
	}
	CL_VKPipeline::~CL_VKPipeline()
	{
		// They are both Destroyed right after Pipeline Creation for now,
		// as such they do not need to be member variables
		//vkDestroyShaderModule(m_CLVKDevice.device(), m_FragShaderModule, nullptr);
		//vkDestroyShaderModule(m_CLVKDevice.device(), m_VertShaderModule, nullptr);
		vkDestroyPipeline(m_CLVKDevice.device(), m_GraphicsPipeline, nullptr);
	}

	void CL_VKPipeline::bind(VkCommandBuffer commandbuffer) const
	{
		vkCmdBindPipeline(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
	}

	void CL_VKPipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(m_CLVKDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			CL_CORE_CRITICAL("Failed To Create Vulkan Shader Module!");
			throw std::runtime_error("Failed to Create Vulkan shader module!");
		}
	}
}