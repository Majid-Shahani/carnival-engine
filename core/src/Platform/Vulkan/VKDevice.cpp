#include <clpch.h>
#include "VKDevice.h"

namespace Carnival {
    
    static const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
    };
    static const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    // Required Device Features :
    // SamplerAnisotropy


    static VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func)
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        else
            return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
    static void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func)
            func(instance, debugMessenger, pAllocator);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Class Member Definitions ///////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VkInstance CL_VKDevice::s_Instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT CL_VKDevice::s_DebugMessenger = VK_NULL_HANDLE;

    CL_VKDevice::CL_VKDevice(GLFWwindow* window) : m_Window(window)
    {
        if (!s_Instance) {
            CreateInstance();
            SetupDebugMessenger();
        }

        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateCommandPool();
        logDeviceProperties();
    }
    CL_VKDevice::~CL_VKDevice()
    {
        vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);
        vkDestroyDevice(m_LogicalDevice, nullptr);


        vkDestroySurfaceKHR(s_Instance, m_Surface, nullptr);
        // Ref Count Then Destroy Instance
        if constexpr (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(s_Instance, s_DebugMessenger, nullptr);
            s_DebugMessenger = VK_NULL_HANDLE;
        }
        vkDestroyInstance(s_Instance, nullptr);
        s_Instance = VK_NULL_HANDLE;
    }

    void CL_VKDevice::createImageWithInfo(
        const VkImageCreateInfo&    imageInfo,
        VkMemoryPropertyFlags       properties,
        VkImage&                    image,
        VkDeviceMemory&             imageMemory) const
    {
        if (vkCreateImage(m_LogicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
        {
            CL_CORE_ERROR("Failed to Create Vulkan Image!");
            throw std::runtime_error("Failed to create vulkan image!");
        }
    }

    // Useful stuff:
    // Device Name, API / Driver Version
    // Device Type
    static constexpr std::string_view deviceType(VkPhysicalDeviceType type) {
        switch (type)
        {
        case 1:
            return "Integrated";
            break;
        case 2:
            return "Discrete";
            break;
        case 3:
            return "Virtual";
            break;
        case 4:
            return "CPU";
            break;
        default:
            return "Other";
        }
    }
    void CL_VKDevice::logDeviceProperties()
    {
        CL_CORE_TRACE("API Version: {0}", m_PhysicalDeviceProperties.apiVersion);
        CL_CORE_TRACE("Driver Version: {0}", m_PhysicalDeviceProperties.driverVersion);
        //CL_CORE_TRACE("Vendor ID: {0}", m_PhysicalDeviceProperties.vendorID);
        //CL_CORE_TRACE("Device ID: {0}", m_PhysicalDeviceProperties.deviceID);
        CL_CORE_TRACE("Device Type: {0}", deviceType(m_PhysicalDeviceProperties.deviceType));
        CL_CORE_TRACE("Device Name: {0}", m_PhysicalDeviceProperties.deviceName);
        
        // LIMITS

        // Sparse Properties
        /*
        CL_CORE_TRACE("Sparse Properties: ");
        CL_CORE_TRACE("2D Block Shape: {0}", m_PhysicalDeviceProperties.sparseProperties.residencyStandard2DBlockShape);
        CL_CORE_TRACE("2D Multisample BlockShape: {0}", m_PhysicalDeviceProperties.sparseProperties.residencyStandard2DMultisampleBlockShape);
        CL_CORE_TRACE("3D Block Shape: {0}", m_PhysicalDeviceProperties.sparseProperties.residencyStandard3DBlockShape);
        CL_CORE_TRACE("Aligned Mip Size: {0}", m_PhysicalDeviceProperties.sparseProperties.residencyAlignedMipSize);
        CL_CORE_TRACE("Non Resident Strict: {0}", m_PhysicalDeviceProperties.sparseProperties.residencyNonResidentStrict);
        */
    }

    void CL_VKDevice::CreateInstance() {
        if (enableValidationLayers && !CheckValidationLayerSupport()) {
            CL_CORE_CRITICAL("Vulkan Validation Layers Requested but not Available!");
            throw std::runtime_error("Validation Layers Requested but not Available!");
        }
        // =========== APP INFO =====================
        VkApplicationInfo appInfo{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "Carnival Engine",
            .applicationVersion = VK_MAKE_API_VERSION(1, 1, 0, 0),
            .pEngineName = "No Engine",
            .engineVersion = VK_MAKE_API_VERSION(1, 1, 0, 0),
            .apiVersion = VK_HEADER_VERSION_COMPLETE,
        };
        // ============ CREATE INFO ================
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = GetRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            PopulateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &s_Instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }

        HasGlfwRequiredInstanceExtensions();
    }
    void CL_VKDevice::SetupDebugMessenger() {
        if constexpr (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        PopulateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(s_Instance, &createInfo, nullptr, &s_DebugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }
    void CL_VKDevice::CreateSurface()
    {
        if (glfwCreateWindowSurface(s_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS)
        {
            CL_CORE_CRITICAL("GLFW Failed to Create Vulkan Surface!");
            throw std::runtime_error("Surface Creation Failed!");
        }
    }
    void CL_VKDevice::PickPhysicalDevice()
    {
        uint32_t deviceCount{};
        vkEnumeratePhysicalDevices(s_Instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            CL_CORE_CRITICAL("Failed To Find GPUs With Vulkan Support!");
            throw std::runtime_error("failed to find GPUs with Vulkan Support!");
        }
        //CL_CORE_TRACE("Device Count: {0}", deviceCount);

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(s_Instance, &deviceCount, devices.data());
        for (const auto& device : devices)
        {
            if (IsDeviceSuitable(device)) {
                m_PhysicalDevice = device;
                break;
            }
        }
        if (m_PhysicalDevice == VK_NULL_HANDLE) {
            CL_CORE_CRITICAL("Failed to find a suitable Vulkan Device!");
            throw std::runtime_error("Failed To Find A Suitable  Vulkan Device!");
        }
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_PhysicalDeviceProperties);
    }
    void CL_VKDevice::CreateLogicalDevice() {
        QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f;
        for (const auto& queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS)
            throw std::runtime_error("Failed To Create Logical Device!");

        vkGetDeviceQueue(m_LogicalDevice, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_LogicalDevice, indices.presentFamily.value(), 0, &m_PresentQueue);
    }
    void CL_VKDevice::CreateCommandPool()
    {
        QueueFamilyIndices qfi = FindQueueFamilies(m_PhysicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Add transient?
        poolInfo.queueFamilyIndex = qfi.graphicsFamily.value();
        if (vkCreateCommandPool(m_LogicalDevice, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
        {
            CL_CORE_CRITICAL("Failed to Create Vulkan Graphics CommandPool!");
            throw std::runtime_error("Failed To Create Graphics Command Pool");
        }
    }

    bool CL_VKDevice::CheckValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }
        return true;
    }
    std::vector<const char*> CL_VKDevice::GetRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void CL_VKDevice::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

        createInfo.messageSeverity =
            //VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
            //| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional
    }
    VKAPI_ATTR VkBool32 VKAPI_CALL CL_VKDevice::debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            CL_CORE_TRACE("{0}:\t{1}\n", ToString(messageType), pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            CL_CORE_INFO("{0}:\t{1}\n", ToString(messageType), pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            CL_CORE_WARN("{0}:\t{1}\n", ToString(messageType), pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            CL_CORE_ERROR("{0}:\t{1}\n", ToString(messageType), pCallbackData->pMessage);
            break;
        }
        return VK_FALSE;
    }
    std::string CL_VKDevice::ToString(VkDebugUtilsMessageTypeFlagsEXT type) {
        switch (type) {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            return "General";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            return "Validation";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            return "Performance";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
            return "Device Address Binding";
            break;
        }
        return "";
    }

    bool CL_VKDevice::IsDeviceSuitable(VkPhysicalDevice device) const
    {
        // Devices that Draw and Present in the same queue
        // Should be prefered for better performance.
        QueueFamilyIndices indices = FindQueueFamilies(device);

        bool extensionsSupported = CheckDeviceExtensionSupport(device);

        bool swapChainAdequate{ false };
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }
        VkPhysicalDeviceFeatures supportedFeatures{};
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete()
            && extensionsSupported
            && swapChainAdequate
            && supportedFeatures.samplerAnisotropy;
    }
    bool CL_VKDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(
            device,
            nullptr,
            &extensionCount,
            availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions)
            requiredExtensions.erase(extension.extensionName);
        return requiredExtensions.empty();
    }
    CL_VKDevice::QueueFamilyIndices CL_VKDevice::FindQueueFamilies(VkPhysicalDevice device) const {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount{};
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i{};
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueCount > 0 
                && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
            VkBool32 presentSupport{ false };
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
            if (queueFamily.queueCount > 0 && presentSupport)  indices.presentFamily = i;

            if (indices.isComplete())    break;
            i++;
        }

        return indices;
    }
    CL_VKDevice::SwapChainSupportDetails CL_VKDevice::QuerySwapChainSupport(VkPhysicalDevice device) const
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

        uint32_t formatCount{};
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);
        if (formatCount) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount{};
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);
        if (presentModeCount) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(
                device,
                m_Surface,
                &presentModeCount,
                details.presentModes.data());
        }

        return details;
    }

    uint32_t CL_VKDevice::findMemoryType(uint32_t typefilter, VkMemoryPropertyFlags properties) const
    {
        VkPhysicalDeviceMemoryProperties props{};
        vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &props);

        for (uint32_t i = 0; i < props.memoryTypeCount; i++)
            if ((typefilter & (1 << i)) 
                && (props.memoryTypes[i].propertyFlags & properties) == properties)
                return i;

        CL_CORE_CRITICAL("Failed to find suitable GPU Memory Type!");
        throw std::runtime_error("Unable to find suitable memory type!");
    }
    VkFormat CL_VKDevice::findSupportedFormat(
        const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features) const {

        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            }
            else if (
                tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }
        CL_CORE_CRITICAL("Failed to find supported format, CL_VKDevice.cpp");
        throw std::runtime_error("failed to find supported format!");

    }

    void CL_VKDevice::createBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& buffermemory,
        VkSharingMode sharingmode) const
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = sharingmode;

        if (vkCreateBuffer(m_LogicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            CL_CORE_CRITICAL("Failed to Create Vulkan Buffer!");
            throw std::runtime_error("Failed to create Vulkan buffer!");
        }

        VkMemoryRequirements memReq{};
        vkGetBufferMemoryRequirements(m_LogicalDevice, buffer, &memReq);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, properties);

        if (vkAllocateMemory(m_LogicalDevice, &allocInfo, nullptr, &buffermemory) != VK_SUCCESS) {
            CL_CORE_CRITICAL("Failed to allocate Vulkan buffer memory!");
            throw std::runtime_error("Failed to allocate buffer memory!");
        }
        vkBindBufferMemory(m_LogicalDevice, buffer, buffermemory, 0);
    }
    void CL_VKDevice::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        // "beginSingleTimeCommands()"
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        // End of function

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        // "endSingleTimeCommands()" begin
        vkEndCommandBuffer(commandBuffer);
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_GraphicsQueue);

        vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &commandBuffer);
    }

    void CL_VKDevice::HasGlfwRequiredInstanceExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        //CL_CORE_TRACE("Available Extensions:");
        std::unordered_set<std::string> available;
        for (const auto& extension : extensions) {
            //CL_CORE_TRACE("\t{0}", extension.extensionName);
            available.insert(extension.extensionName);
        }

        //CL_CORE_TRACE("Required Extensions:");
        auto requiredExtensions = GetRequiredExtensions();
        for (const auto& required : requiredExtensions) {
            //CL_CORE_TRACE("\t{0}", required);
            if (available.find(required) == available.end()) {
                CL_CORE_CRITICAL("Missing Required GLFW Extension(s)!");
                throw std::runtime_error("Missing required glfw extension");
            }
        }
    }
}