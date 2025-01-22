#pragma once

#include "CL_VKDevice.h"

namespace Carnival {

	class CL_VKSwapChain {
    public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		CL_VKSwapChain(CL_VKDevice& deviceRef, VkExtent2D extent, bool VSync = true);
		CL_VKSwapChain(CL_VKDevice& deviceRef, VkExtent2D extent, std::shared_ptr<CL_VKSwapChain> previous, bool VSync = true);
		~CL_VKSwapChain();

		CL_VKSwapChain(const CL_VKSwapChain&) = delete;
		CL_VKSwapChain& operator=(const CL_VKSwapChain&) = delete;
		CL_VKSwapChain(CL_VKSwapChain&&) = delete;
		CL_VKSwapChain& operator=(CL_VKSwapChain&&) = delete;

		VkFramebuffer	getFrameBuffer(int index) const { return m_SwapChainFramebuffers[index]; }
		VkRenderPass	getRenderPass() const			{ return m_RenderPass; }
		VkImageView		getImageView(int index) const	{ return m_SwapChainImageViews[index]; }
		size_t			getImageCount() const			{ return m_SwapChainImages.size(); }
		VkFormat		getSwapChainImageFormat() const { return m_SwapChainImageFormat; }
		VkExtent2D		getSwapChainExtent() const		{ return m_SwapChainExtent; }
		uint32_t		getWidth() const				{ return m_SwapChainExtent.width; }
		uint32_t		getHeight() const				{ return m_SwapChainExtent.height; }

		float extentAspectRatio() const {
			return static_cast<float>(m_SwapChainExtent.width) / static_cast<float>(m_SwapChainExtent.height);
		}
		VkFormat findDepthFormat();

		VkResult acquireNextImage(uint32_t* imageIndex, const uint32_t& currentFrame);
		VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex, const uint32_t& currentFrame);

    private:
		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
		std::shared_ptr<CL_VKSwapChain> m_OldSwapChain;
		CL_VKDevice& m_Device;
		VkExtent2D m_WindowExtent;

		VkFormat	m_SwapChainImageFormat = VK_FORMAT_UNDEFINED;
		VkExtent2D	m_SwapChainExtent{};

		std::vector<VkImage>		m_DepthImages;
		std::vector<VkDeviceMemory> m_DepthImageMemorys;
		std::vector<VkImageView>	m_DepthImageViews;

		std::vector<VkImage>		m_SwapChainImages; // Cleaned up on swap chain destruction
		std::vector<VkImageView>	m_SwapChainImageViews;

		std::vector<VkFramebuffer>	m_SwapChainFramebuffers;
		VkRenderPass				m_RenderPass = VK_NULL_HANDLE;
		
		std::vector<VkSemaphore>	m_ImageAvailableSemaphores;
		std::vector<VkSemaphore>	m_RenderFinishedSemaphores;
		std::vector<VkFence>		m_InFlightFences;
		std::vector<VkFence>		imagesInFlight;

		bool m_VSync;
		
		void init();
		void createSwapChain();
		void createImageViews();
		void createDepthResources();
		void createRenderPass();
		void createFramebuffers();
		void createSyncObjects();
		
		static VkSurfaceFormatKHR	chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR			chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& modes) const;
		VkExtent2D					chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
	};
}