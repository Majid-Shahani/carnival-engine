#include <clpch.h>
#include "CL_VKSwapChain.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////// BIG FUCKING ISSUE ///////////////////////////////////////////////////////////////////////////////
////////////// INFLIGHT FENCES NOT INITIALIZED NOR DESTROYED, DEPTH IMAGES NOT CREATED /////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Carnival {
	CL_VKSwapChain::CL_VKSwapChain(
        CL_VKDevice&    deviceRef,
        VkExtent2D      extent,
        bool            VSync) 
		: m_Device{deviceRef}, m_WindowExtent{extent}, m_VSync{VSync}
	{
        init();
	}

    CL_VKSwapChain::CL_VKSwapChain(
        CL_VKDevice& deviceRef,
        VkExtent2D extent,
        std::shared_ptr<CL_VKSwapChain> previous,
        bool VSync)
        : m_Device{ deviceRef }, m_WindowExtent{ extent }, m_OldSwapChain{previous}, m_VSync{VSync}
    {
        init();
        m_OldSwapChain = nullptr;
    }

    CL_VKSwapChain::~CL_VKSwapChain()
    {
        for (auto framebuffer : m_SwapChainFramebuffers)
            vkDestroyFramebuffer(m_Device.device(), framebuffer, nullptr);


        for (auto imageview : m_SwapChainImageViews)
            vkDestroyImageView(m_Device.device(), imageview, nullptr);
        m_SwapChainImageViews.clear();

        if (m_SwapChain) {
            vkDestroySwapchainKHR(m_Device.device(), m_SwapChain, nullptr);
            m_SwapChain = nullptr;
        }

        vkDestroyRenderPass(m_Device.device(), m_RenderPass, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(m_Device.device(), m_ImageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(m_Device.device(), m_RenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(m_Device.device(), m_InFlightFences[i], nullptr);
        }
    }

    void CL_VKSwapChain::init()
    {
        createSwapChain();
        createImageViews();
        createRenderPass();
        createFramebuffers();
        createSyncObjects();
    }

    void CL_VKSwapChain::createSwapChain()
    {
        CL_VKDevice::SwapChainSupportDetails swapChainSupport = m_Device.getSwapChainSupport();

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D swapExtent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0
            && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{
            .sType =            VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext =            nullptr,
            .flags =            0,
            .surface =          m_Device.surface(),
            .minImageCount =    imageCount,
            .imageFormat =      surfaceFormat.format,
            .imageColorSpace =  surfaceFormat.colorSpace,
            .imageExtent =      swapExtent,
            .imageArrayLayers = 1,
            .imageUsage =       VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        };

        auto indices = m_Device.findPhysicalQueueFamilies();
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = (m_OldSwapChain.get() ? m_OldSwapChain->m_SwapChain : VK_NULL_HANDLE);

        if (vkCreateSwapchainKHR(m_Device.device(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
            CL_CORE_CRITICAL("Failed To Create Vulkan SwapChain!");
            throw std::runtime_error("Failed To Create SwapChain!");
        }
        // Store Data
        vkGetSwapchainImagesKHR(m_Device.device(), m_SwapChain, &imageCount, nullptr);
        m_SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device.device(), m_SwapChain, &imageCount, m_SwapChainImages.data());
        m_SwapChainImageFormat = surfaceFormat.format;
        m_SwapChainExtent = swapExtent;
    }
    void CL_VKSwapChain::createImageViews()
    {
        m_SwapChainImageViews.resize(m_SwapChainImages.size());
        for (size_t i = 0; i < m_SwapChainImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_SwapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_SwapChainImageFormat;
            
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            
            createInfo.subresourceRange.aspectMask =     VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel =   0;
            createInfo.subresourceRange.levelCount =     1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount =     1;
            
            if (vkCreateImageView(m_Device.device(), &createInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS) {
                CL_CORE_CRITICAL("Vulkan ImageView Creation Failed!");
                throw std::runtime_error("Image View Creation Failed!");
            }
        }
    }
    void CL_VKSwapChain::createRenderPass()
    {
        // Attachments
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_SwapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // Attachment References
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0; // Index of Attachment Description Array
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // Subpass
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        // Subpass Dependencies
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;

        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;

        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        // Render Pass
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        if (vkCreateRenderPass(m_Device.device(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
            CL_CORE_CRITICAL("Failed To Create Vulkan RenderPass!");
            throw std::runtime_error("Failed To Create Render Pass!");
        }
    }

    void CL_VKSwapChain::createDepthResources() {
        VkFormat depthFormat = findDepthFormat();
        VkExtent2D swapChainExtent = getSwapChainExtent();

        m_DepthImages.resize(getImageCount());
        m_DepthImageMemorys.resize(getImageCount());
        m_DepthImageViews.resize(getImageCount());

        for (int i = 0; i < m_DepthImages.size(); i++) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType =           VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType =       VK_IMAGE_TYPE_2D;
            imageInfo.extent.width =    swapChainExtent.width;
            imageInfo.extent.height =   swapChainExtent.height;
            imageInfo.extent.depth =    1;
            imageInfo.mipLevels =       1;
            imageInfo.arrayLayers =     1;
            imageInfo.format =          depthFormat;
            imageInfo.tiling =          VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout =   VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage =           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            imageInfo.samples =         VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode =     VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.flags =           0;

            m_Device.createImageWithInfo(
                imageInfo,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                m_DepthImages[i],
                m_DepthImageMemorys[i]);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = m_DepthImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = depthFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_Device.device(), &viewInfo, nullptr, &m_DepthImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }

    void CL_VKSwapChain::createFramebuffers()
    {
        m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());
        for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
            VkImageView attachments[] = { m_SwapChainImageViews[i] };
            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType =             VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass =        m_RenderPass;
            framebufferInfo.attachmentCount =   1;
            framebufferInfo.pAttachments =      attachments;
            framebufferInfo.width =             m_SwapChainExtent.width;
            framebufferInfo.height =            m_SwapChainExtent.height;
            framebufferInfo.layers =            1;

            if (vkCreateFramebuffer(m_Device.device(), &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS) {
                CL_CORE_CRITICAL("Vulkan Framebuffer Creation Failed!");
                throw std::runtime_error("Framebuffer Creation Failed!");
            }
        }
    }
    void CL_VKSwapChain::createSyncObjects()
    {
        m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(getImageCount(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (rsize_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(m_Device.device(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS
                || vkCreateSemaphore(m_Device.device(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS
                || vkCreateFence(m_Device.device(), &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS) {
                    CL_CORE_CRITICAL("Failed to create sync objects for frame at index: {0}", i);
                    throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    VkResult CL_VKSwapChain::acquireNextImage(uint32_t* imageIndex, const uint32_t& currentFrame)
    {
        vkWaitForFences(m_Device.device(), 1, &m_InFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        VkResult result = vkAcquireNextImageKHR(
            m_Device.device(),
            m_SwapChain,
            std::numeric_limits<uint64_t>::max(),
            m_ImageAvailableSemaphores[currentFrame],  // must be a not signaled semaphore
            VK_NULL_HANDLE,
            imageIndex);

        vkResetFences(m_Device.device(), 1, &m_InFlightFences[currentFrame]);
        return result;
    }

    VkResult CL_VKSwapChain::submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex, const uint32_t& currentFrame)
    {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffers;

        VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_Device.graphicsQueue(), 1, &submitInfo, m_InFlightFences[currentFrame]) != VK_SUCCESS)
            throw std::runtime_error("Failed To Submit Draw Command Buffer!");

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { m_SwapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = imageIndex;
        presentInfo.pResults = nullptr; // To check return values of multiple swap chains

        auto result = vkQueuePresentKHR(m_Device.presentQueue(), &presentInfo);
        return result;
    }

    VkPresentModeKHR CL_VKSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& modes) const
    {   // Automatic Selection
        if (m_VSync == true) {
            if (std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != modes.end())
                return VK_PRESENT_MODE_MAILBOX_KHR; // Triple-Buffering, Low Latency, No Tearing
            if (std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_FIFO_RELAXED_KHR) != modes.end())
                return VK_PRESENT_MODE_FIFO_RELAXED_KHR; // FIFO but if buffer busy, Present as is

            return VK_PRESENT_MODE_FIFO_KHR; // Power Efficient
        }
        else {
            if (std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) != modes.end())
                return VK_PRESENT_MODE_IMMEDIATE_KHR; // Present ASAP, No Latency, Tearing
            else throw std::runtime_error("Device Does Not Support Immediate Present Mode! ( VSYNC = ON )");

            return VK_PRESENT_MODE_FIFO_KHR;
        }
    }
    VkExtent2D CL_VKSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent;
        else {
            VkExtent2D actualExtent = m_WindowExtent;
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
            return actualExtent;
        }
    }
    VkSurfaceFormatKHR CL_VKSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;
        }
        return availableFormats[0];
    }
    VkFormat CL_VKSwapChain::findDepthFormat() {
        return m_Device.findSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }
}