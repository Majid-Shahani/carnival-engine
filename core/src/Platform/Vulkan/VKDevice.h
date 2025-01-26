#pragma once

#include <Platform/Window/WindowImpl.h>

namespace Carnival {

#ifdef CL_DEBUG
    static constexpr bool enableValidationLayers = true;
#else
    static constexpr bool enableValidationLayers = false;
#endif

    class CL_VKDevice {
    public:

        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;
            bool isComplete() const {
                return graphicsFamily.has_value() && presentFamily.has_value();
            }
        };
        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR capabilities{};
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };


        CL_VKDevice(GLFWwindow* window);
        ~CL_VKDevice();

        CL_VKDevice(const CL_VKDevice&) = delete;
        CL_VKDevice& operator=(const CL_VKDevice&) = delete;
        CL_VKDevice(CL_VKDevice&&) = delete;
        CL_VKDevice& operator=(CL_VKDevice&&) = delete;


        VkCommandPool getCommandPool() const { return m_CommandPool; }
        GLFWwindow* getWindow() const { return m_Window; }
        VkDevice device() const { return m_LogicalDevice; }
        VkSurfaceKHR surface() const { return m_Surface; }
        VkQueue graphicsQueue() const{ return m_GraphicsQueue; }
        VkQueue presentQueue() const{ return m_PresentQueue; }

        SwapChainSupportDetails getSwapChainSupport() const { return QuerySwapChainSupport(m_PhysicalDevice); }
        QueueFamilyIndices findPhysicalQueueFamilies() { return FindQueueFamilies(m_PhysicalDevice); }

        uint32_t findMemoryType(uint32_t typefilter, VkMemoryPropertyFlags properties) const;
        VkFormat findSupportedFormat(
            const std::vector<VkFormat>& candidates,
            VkImageTiling tiling,
            VkFormatFeatureFlags features) const;

        // Buffer Helper Functions
        void createBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VkDeviceMemory& buffermemory,
            VkSharingMode sharingmode = VK_SHARING_MODE_EXCLUSIVE) const;
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copyBufferToImage(
            VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

        void createImageWithInfo(
            const VkImageCreateInfo& imageInfo,
            VkMemoryPropertyFlags properties,
            VkImage& image,
            VkDeviceMemory& imageMemory) const;

        void logDeviceProperties();
        VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
    private:
        static void CreateInstance();
        static void SetupDebugMessenger();
        void CreateSurface();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateCommandPool();

        // helper functions
        bool IsDeviceSuitable(const VkPhysicalDevice device) const;
        static std::vector<const char*> GetRequiredExtensions();
        static bool CheckValidationLayerSupport();
        static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice device) const;
        QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice device) const;


        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );
        static std::string ToString(VkDebugUtilsMessageTypeFlagsEXT type);
        static bool CheckDeviceExtensionSupport(const VkPhysicalDevice device);

        static void HasGlfwRequiredInstanceExtensions();

        static VkInstance s_Instance;
        static VkDebugUtilsMessengerEXT s_DebugMessenger;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        GLFWwindow* m_Window;
        VkCommandPool m_CommandPool;

        VkDevice m_LogicalDevice;
        VkSurfaceKHR m_Surface;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;
    };
}