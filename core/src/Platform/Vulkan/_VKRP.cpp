#include <clpch.h>
/*
#include "VKRenderSystem.h"
#include "VKBuffer.h"

#include <glm/gtc/matrix_transform.hpp>
#include "VKRenderer.h"

namespace Carnival {

	CL_VKRenderer::CL_VKRenderer(GLFWwindow* window, bool VSync) : m_Window{ window }, m_VSync{ VSync }, m_Device{window}
	{
		init();

		createPipelineLayout(m_GlobalSetLayout->getLayout());
		recreateSwapChain();
		createPipeline();
		createCommandBuffers();
	}
	CL_VKRenderer::~CL_VKRenderer()
	{
		vkDeviceWaitIdle(m_Device.device());
        // Destroy members in order
		m_GlobalPool.reset();
        vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
	}


	
	void CL_VKRenderer::loadModel(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		m_Model = std::make_unique<CL_VKModel>(m_Device, vertices, indices);
	}

	void CL_VKRenderer::createPipelineLayout(VkDescriptorSetLayout setLayout) 
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
	void CL_VKRenderer::recreateSwapChain()
	{
		int width{}, height{};

		glfwGetFramebufferSize(m_Window, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(m_Window, &width, &height);
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(m_Device.device());
		VkExtent2D extent{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		if (!m_SwapChain)
			m_SwapChain = std::make_unique<CL_VKSwapChain>(m_Device, extent, m_VSync);
		else {
			m_SwapChain = std::make_unique<CL_VKSwapChain>(m_Device, extent, std::move(m_SwapChain), m_VSync);
			if (m_SwapChain->getImageCount() != m_CommandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		//createPipeline(); // if new swapchain is compatible with the previous one, there is no need to recreate the pipeline
	}
	void CL_VKRenderer::createPipeline()
	{
		CL_CORE_ASSERT(m_SwapChain.get(), "Cannot create Pipeline before swapchain!");
		CL_CORE_ASSERT(m_PipelineLayout, "Cannot create Pipeline before Pipeline Layout!");
		PipelineConfigInfo configInfo(m_SwapChain->getWidth(), m_SwapChain->getHeight());
		configInfo.renderPass = m_SwapChain->getRenderPass();
		configInfo.pipelineLayout = m_PipelineLayout;
		
		std::filesystem::path vertPath("./src/shaders/shader.vert.spv");
		std::filesystem::path fragPath("./src/shaders/shader.frag.spv");

		m_Pipeline = std::make_unique<CL_VKPipeline>(
			m_Device,
			vertPath,
			fragPath,
			configInfo);
	}

	void CL_VKRenderer::createCommandBuffers()
	{
		m_CommandBuffers.resize(m_SwapChain->MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_Device.getCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Primary Buffer vs Secondary Buffer to be called from primary buffer
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_Device.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
			CL_CORE_CRITICAL("Failed to Allocate Command Buffers");
			throw std::runtime_error("Failed To Allocate Command Buffers!");
		}
	}
	void CL_VKRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(m_Device.device(), m_Device.getCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}

	void CL_VKRenderer::clear()
	{
		m_ImageIndex = 0;
		auto result = m_SwapChain->acquireNextImage(&m_ImageIndex, m_CurrentFrame);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swapchain image!");
		}
		vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
	}
	void CL_VKRenderer::drawFrame()
	{
		recordCommandBuffer();
		updateUniformBuffer();
	}
	void CL_VKRenderer::swapBuffers()
	{
		// Submit Command buffer
		auto result = m_SwapChain->submitCommandBuffers(&m_CommandBuffers[m_CurrentFrame], &m_ImageIndex, m_CurrentFrame);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_FramebufferResized) {
			m_FramebufferResized = false;
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to submit command buffer!");
		}
		m_CurrentFrame = (m_CurrentFrame + 1) % CL_VKSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void CL_VKRenderer::recordCommandBuffer()
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // optional
		beginInfo.pInheritanceInfo = nullptr; // optional
		if (vkBeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_SwapChain->getRenderPass();
		renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(m_ImageIndex);
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

		VkClearValue clearColor = { {{ 0.01f, 0.01f, 0.01f, 1.0f }} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(m_CommandBuffers[m_CurrentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_SwapChain->getWidth());
		viewport.height = static_cast<float>(m_SwapChain->getHeight());
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_CommandBuffers[m_CurrentFrame], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_SwapChain->getSwapChainExtent();
		vkCmdSetScissor(m_CommandBuffers[m_CurrentFrame], 0, 1, &scissor);


		m_Pipeline->bind(m_CommandBuffers[m_CurrentFrame]);
		m_Model->bind(m_CommandBuffers[m_CurrentFrame]);


		vkCmdBindDescriptorSets(
			m_CommandBuffers[m_CurrentFrame],
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_PipelineLayout,
			0, 1, // first descriptor set to be bound, and the number of sets to be bound
			&m_GlobalDescriptorSets[m_CurrentFrame],
			0, nullptr); // dynamic offsets


		for (int j = 0; j < 1; j++) {
			PushConstantData push{
				.offset = {0.0f, j * 0.35f},
				.color = {0.1f * j * j, 0.3f * j, 0.2f + 0.2f * j}
			};

			vkCmdPushConstants(
				m_CommandBuffers[m_CurrentFrame],
				m_PipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PushConstantData),
				&push
			);

			m_Model->draw(m_CommandBuffers[m_CurrentFrame]);
		}


		vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);
		if (vkEndCommandBuffer(m_CommandBuffers[m_CurrentFrame]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");
	}
	void CL_VKRenderer::updateUniformBuffer()
	{
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		Carnival::UniformBufferObject ubo{
			.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			//+ glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
			.view = glm::lookAt(glm::vec3(0.0f, -1.0f, 1.0f), glm::vec3{}, glm::vec3{0.0f, 1.0f, 0.0f}),
			.proj = glm::perspective(glm::radians(75.0f), (float)m_SwapChain->getWidth() / (float)m_SwapChain->getHeight(), 0.1f, 10.0f)
		};
		ubo.proj[1][1] *= -1; // do this for vulkan, not for OGL
		m_UniformBuffers[m_CurrentFrame]->writeToBuffer(&ubo);
		m_UniformBuffers[m_CurrentFrame]->flush();
	}


	void CL_VKRenderer::setSwapInterval(bool VSync)
	{
		m_VSync = VSync;
		recreateSwapChain();
	}
	void CL_VKRenderer::framebufferResizeCallback()
	{
		m_FramebufferResized = true;
	}
	*/