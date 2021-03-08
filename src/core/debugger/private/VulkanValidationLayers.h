#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace vkLayers
{

    class VulkanValidationLayer
    {
    public:
        const bool EnableValidationLayers;
        const std::vector<const char*> ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
        VkDebugUtilsMessengerEXT DebugMessenger;
        VkDebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo;

        VulkanValidationLayer();
        void CleanUpValidationLayers(VkInstance vulkanInstance);

         
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
                const VkDebugUtilsMessengerCreateInfoEXT* createInfo, 
                const VkAllocationCallbacks* pAllocator);
        bool CheckValidationLayerSupport();
        void SetUpDebugMessenger(VkInstance vulkanInstance);

        
        void LogVulkanExtensions(const std::vector<VkExtensionProperties>& extensions, const std::vector<const char*> requiredExtensions);
        void LogGpuExtensions(const std::vector<VkExtensionProperties>& extensions, const std::vector<const char*> requiredExtensions);

    };


     uint32_t DebugUtilsMessengerCallbackEXT(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT  messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

 
    
}

