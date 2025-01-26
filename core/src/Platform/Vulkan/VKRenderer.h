#pragma once
#include <clpch.h>
#include <carnival/Renderer/Renderer.h>

#include "VKDevice.h"
#include "VKSwapChain.h"
#include "VKDescriptors.h"

struct GLFWwindow;

namespace Carnival {
	class CL_VKRenderer{
	public:

		CL_VKRenderer(GLFWwindow* window, CL_VKDevice& device);
		~CL_VKRenderer();

		CL_VKRenderer(const CL_VKRenderer&) = delete;
		CL_VKRenderer& operator=(const CL_VKRenderer&) = delete;

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		void framebufferResizeCallback();

		VkRenderPass	getSwapChainRenderPass() const { return m_SwapChain->getRenderPass(); }
		VkExtent2D		getSwapChainExtent() const { return m_SwapChain->getSwapChainExtent(); }
		VkCommandBuffer getCurrentCommandBuffer() const;
		uint32_t		getFrameIndex() const { return m_CurrentFrame; }
		bool			isFrameStarted() const { return m_IsFrameStarted; }
	private:

		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		GLFWwindow* m_Window{ nullptr };
		CL_VKDevice& m_Device;

		std::unique_ptr<CL_VKSwapChain> m_SwapChain;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		uint32_t m_CurrentFrame{};
		uint32_t m_ImageIndex{};
		bool m_FramebufferResized = false;
		bool m_IsFrameStarted = false;

	};
}