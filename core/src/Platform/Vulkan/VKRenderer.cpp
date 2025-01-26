#include <clpch.h>
#include "VKRenderer.h"

namespace Carnival {

	CL_VKRenderer::CL_VKRenderer(GLFWwindow* window, CL_VKDevice& device) : m_Window{ window }, m_Device{ device }
	{
		recreateSwapChain();
		createCommandBuffers();
	}
	CL_VKRenderer::~CL_VKRenderer()
	{
		vkDeviceWaitIdle(m_Device.device());
		// Destroy members in order
		freeCommandBuffers();
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
			m_SwapChain = std::make_unique<CL_VKSwapChain>(m_Device, extent);
		else {
			std::shared_ptr<CL_VKSwapChain> oldSwapChain = std::move(m_SwapChain);
			m_SwapChain = std::make_unique<CL_VKSwapChain>(m_Device, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*m_SwapChain.get()))
			{
				CL_CORE_CRITICAL("SwapChain Image or Depth Format has changed.\nSwapChains Are incompatible!");
			}
		}
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

	VkCommandBuffer CL_VKRenderer::beginFrame()
	{
		CL_CORE_ASSERT(!m_IsFrameStarted, "Can't Call beginFrame while frame is in progress.");
		auto result = m_SwapChain->acquireNextImage(&m_ImageIndex, m_CurrentFrame);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swapchain image!");
		}
		m_IsFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();
		vkResetCommandBuffer(commandBuffer, 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // optional
		beginInfo.pInheritanceInfo = nullptr; // optional
		if (vkBeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		return commandBuffer;
	}
	void CL_VKRenderer::endFrame()
	{
		CL_CORE_ASSERT(m_IsFrameStarted, "Can't call endFrame while frame not in progress!");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");

		// Submit Command buffer
		auto result = m_SwapChain->submitCommandBuffers(&commandBuffer, &m_ImageIndex, m_CurrentFrame);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_FramebufferResized) {
			m_FramebufferResized = false;
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to submit command buffer!");
		}

		m_IsFrameStarted = false;
		m_CurrentFrame = (m_CurrentFrame + 1) % CL_VKSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void CL_VKRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		CL_CORE_ASSERT(m_IsFrameStarted, "Can't Call beginSwapChainRenderPass if frame is not in progress");
		CL_CORE_ASSERT(commandBuffer == getCurrentCommandBuffer(), "Can't begin RenderPass of another frame's commandBuffer");


		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_SwapChain->getRenderPass();
		renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(m_ImageIndex);
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

		VkClearValue clearColor = { {{ 0.01f, 0.01f, 0.01f, 1.0f }} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_SwapChain->getWidth());
		viewport.height = static_cast<float>(m_SwapChain->getHeight());
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_SwapChain->getSwapChainExtent();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}
	void CL_VKRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		CL_CORE_ASSERT(m_IsFrameStarted, "Can't Call endSwapChainRenderPass if frame is not in progress");
		CL_CORE_ASSERT(commandBuffer == getCurrentCommandBuffer(), "Can't end RenderPass of another frame's commandBuffer");

		vkCmdEndRenderPass(commandBuffer);
	}

	void CL_VKRenderer::framebufferResizeCallback()
	{
		m_FramebufferResized = true;
	}

	VkCommandBuffer CL_VKRenderer::getCurrentCommandBuffer() const
	{
		CL_CORE_ASSERT(m_IsFrameStarted, "Cannot get CommandBuffer when frame is not in progress!");
		return m_CommandBuffers[m_CurrentFrame];
	}
}