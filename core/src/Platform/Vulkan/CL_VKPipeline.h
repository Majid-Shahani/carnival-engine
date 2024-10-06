#pragma once
#include <carnival/Renderer/Renderer.h>

#include "CL_VKDevice.h"
#include <string>

namespace Carnival {

    struct PipelineConfigInfo {
        PipelineConfigInfo(uint32_t width, uint32_t height) {
            // Input Assembly
            inputAssemblyInfo = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .primitiveRestartEnable = VK_FALSE,
            };

            // Viewports And Scissors
            viewport = {
                .x = 0.0f,
                .y = 0.0f,
                .width = (float)width, // convert?
                .height = (float)height, // convert?
                .minDepth = 0.0f,
                .maxDepth = 1.0f,
            };
            scissor = {
                .offset = { 0,0 },
                .extent = {width, height}
            };

            // Rasterizer
            rasterizer = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .depthClampEnable = VK_FALSE,
                .rasterizerDiscardEnable = VK_FALSE,
                .polygonMode = VK_POLYGON_MODE_FILL,
                .cullMode = VK_CULL_MODE_BACK_BIT,
                .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                .depthBiasEnable = VK_FALSE,
                .depthBiasConstantFactor = 0.0f,
                .depthBiasClamp = 0.0f,
                .depthBiasSlopeFactor = 0.0f,
                .lineWidth = 1.0f
            };

            // Multi Sampling
            multisampling = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                .sampleShadingEnable = VK_FALSE,
                .minSampleShading = 1.0f, // Optional
                .pSampleMask = nullptr, // Optional
                .alphaToCoverageEnable = VK_FALSE, // Optional
                .alphaToOneEnable = VK_FALSE, // Optional
            };

            // Depth Stencil
            depthStencil = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .pNext = nullptr,
                .depthTestEnable = VK_TRUE,
                .depthWriteEnable = VK_TRUE,
                .depthCompareOp = VK_COMPARE_OP_LESS,
                .depthBoundsTestEnable = VK_FALSE,
                .stencilTestEnable = VK_FALSE,
                .front = {},
                .back = {},
                .minDepthBounds = 0.0f,
                .maxDepthBounds = 1.0f
            };

            // Color Blending
            colorBlendAttachment = {
                .blendEnable = VK_FALSE,
                .srcColorBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
                .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
                .colorBlendOp = VK_BLEND_OP_ADD, // Optional
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
                .alphaBlendOp = VK_BLEND_OP_ADD, // Optional
                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
            };
            colorBlending = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .logicOpEnable = VK_FALSE, // For bitwise Blending
                .logicOp = VK_LOGIC_OP_COPY, // Optional
                .attachmentCount = 1,
                .pAttachments = &colorBlendAttachment,
            };
            colorBlending.blendConstants[0] = 0.0f;
            colorBlending.blendConstants[1] = 0.0f;
            colorBlending.blendConstants[2] = 0.0f;
            colorBlending.blendConstants[3] = 0.0f;
        }

        VkViewport viewport{};
        VkRect2D scissor{};
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        VkPipelineMultisampleStateCreateInfo multisampling{};
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        VkPipelineDepthStencilStateCreateInfo depthStencil{};

        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
	};

	class CL_VKPipeline {
	public:
		CL_VKPipeline(
			CL_VKDevice& device,
			const std::string& vertShaderPath, const std::string& fragShaderPath,
			const PipelineConfigInfo& configInfo);
        ~CL_VKPipeline();

		CL_VKPipeline(const CL_VKPipeline&) = delete;
		CL_VKPipeline& operator=(const CL_VKPipeline&) = delete;
		CL_VKPipeline(CL_VKPipeline&&) = delete;
		CL_VKPipeline& operator=(CL_VKPipeline&&) = delete;

        void bind(VkCommandBuffer commandbuffer) const;

	private:
		CL_VKDevice& m_CLVKDevice;
		VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;

		void createGraphicsPipeline(
			const std::string& vertShaderPath, const std::string& fragShaderPath,
			const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
	};
}