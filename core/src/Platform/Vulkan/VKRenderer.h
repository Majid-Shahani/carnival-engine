#pragma once
#include <carnival/Renderer/Renderer.h>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Carnival {
	class VulkanRenderer : public Renderer {
	public:
        VulkanRenderer(GLFWwindow* window, bool VSync = true) : m_Window{ window }, m_VSync(VSync)
        {}
        ~VulkanRenderer();

        static bool InstanceExists();

	private:
        static bool s_InstanceExists;
        static uint32_t s_RendererCount;
//===================================================== STRUCTS =========================================================//
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
        struct Vertex {
            glm::vec2 pos;
            glm::vec3 color;

            static VkVertexInputBindingDescription getBindingDescription() {
                VkVertexInputBindingDescription bindingDescription{};
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(Vertex);
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                return bindingDescription;
            }
            static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescription() {
                std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
                attributeDescriptions[0].binding = 0;
                attributeDescriptions[0].location = 0;
                attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
                attributeDescriptions[0].offset = offsetof(Vertex, pos);

                attributeDescriptions[1].binding = 0;
                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[1].offset = offsetof(Vertex, color);
                return attributeDescriptions;
            }
        };
//===================================================== VARIABLES =======================================================//
        bool m_VSync;

        const std::vector<Vertex> m_Vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
        };
        const std::vector<uint16_t> m_Indices = {
            0, 1, 2, 2, 3, 0
        };

        GLFWwindow* m_Window = nullptr; // Platform Dependent
        bool m_FramebufferResized = false;

        static VkInstance s_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE; // should be static
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE; // Implicitly Destroyed on Instance Destruction
        VkDevice m_LogicalDevice = VK_NULL_HANDLE;

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE; // Implicitly Destroyed on Logical Device Destruction
        VkQueue m_PresentQueue = VK_NULL_HANDLE;

        // usually only one swap chain at a time // static?
        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
        std::vector<VkImage> m_SwapChainImages; // Cleaned up on swap chain destruction
        VkFormat m_SwapChainImageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D m_SwapChainExtent{};
        std::vector<VkImageView> m_SwapChainImageViews;
        std::vector<VkFramebuffer> m_SwapChainFramebuffers;

        VkRenderPass m_RenderPass = VK_NULL_HANDLE;
        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
        VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;

        VkCommandPool m_CommandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> m_CommandBuffers; // Implicitly Destroyed on command pool Destruction

        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;
        uint32_t m_CurrentFrame = 0;


        VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
        VkBuffer m_IndexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_VertexBufferMemory = VK_NULL_HANDLE;
        VkDeviceMemory m_IndexBufferMemory = VK_NULL_HANDLE;
//======================================== Functions ================================================================//
        void Cleanup();

        void DrawFrame();

        // Instance // should instance be static?
        static void CreateInstance();

        static bool CheckValidationLayerSupport(); 
        static std::vector<const char*> GetRequiredExtensions();
        static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );
        static std::string ToString(VkDebugUtilsMessageTypeFlagsEXT type);

        void SetupDebugMessenger();
        void CreateSurface();

        void PickPhysicalDevice();
        bool IsDeviceSuitable(const VkPhysicalDevice device) const;
        QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice device) const;
        static bool CheckDeviceExtensionSupport(const VkPhysicalDevice device);

        SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice device) const;
        static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& modes) const;
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

        void RecreateSwapChain();
        void CleanupSwapChain();

        void CreateLogicalDevice();
        void CreateSwapChain();
        void CreateImageViews();
        void CreateRenderPass();
        
        void CreateGraphicsPipeline();
        VkShaderModule CreateShaderModule(const std::vector<char>& code) const;

        void CreateFramebuffers();
        void CreateVertexBuffer();
        void CreateIndexBuffer();

        uint32_t FindMemoryType(uint32_t typefilter, VkMemoryPropertyFlags properties) const;
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
            VkBuffer& buffer, VkDeviceMemory& buffermemory, VkSharingMode sharingmode = VK_SHARING_MODE_EXCLUSIVE);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void CreateCommandPool();
        void CreateCommandBuffers();
        void RecordCommandBuffer(VkCommandBuffer commandbuffer, uint32_t imageindex);

        void CreateSyncObjects();

        // Inherited via Renderer
        virtual void Init() override;
        virtual void SetSwapInterval(bool VSync) override;
        virtual void FramebufferResizeCallback() override;
};
}