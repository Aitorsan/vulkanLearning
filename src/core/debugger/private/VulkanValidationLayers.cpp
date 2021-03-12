#include "VulkanValidationLayers.h"

#include <cstring>
#include <string>
#include "core/debugger/public/Logger.h"

namespace vkLayers
{

    uint32_t DebugUtilsMessengerCallbackEXT(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT  messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        if (messageSeverity & (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT))
        {
            VAL_LAYER((std::string(pCallbackData->pMessage)+"\n").c_str())

        }
        return VK_FALSE;
    }


    VulkanValidationLayer::VulkanValidationLayer()
#ifdef NDEBUG
        : EnableValidationLayers{ false }
        , ValidationLayers {}
#else
        : EnableValidationLayers{ true }
        , ValidationLayers { "VK_LAYER_KHRONOS_validation" }
#endif
        , DebugMessenger{}
        , DebugMessengerCreateInfo{}
    {
        //set up messenger create info
        DebugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        DebugMessengerCreateInfo.messageSeverity =
              VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        DebugMessengerCreateInfo.messageType = 
             VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        DebugMessengerCreateInfo.pfnUserCallback = &vkLayers::DebugUtilsMessengerCallbackEXT;
        DebugMessengerCreateInfo.pUserData = nullptr;// optional
    }


    void VulkanValidationLayer::CleanUpValidationLayers(VkInstance vulkanInstance)
    {
        if (EnableValidationLayers) 
        {
            auto DestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
            if (DestroyDebugUtilsMessengerEXT != nullptr) {
                DestroyDebugUtilsMessengerEXT(vulkanInstance, DebugMessenger, nullptr);
            }
        }
    }

    VkResult VulkanValidationLayer::CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
        const VkAllocationCallbacks* pAllocator)
    {
        auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        return (vkCreateDebugUtilsMessengerEXT != nullptr)
               ? vkCreateDebugUtilsMessengerEXT(instance, createInfo, pAllocator, &this->DebugMessenger)
               : VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    bool VulkanValidationLayer::CheckValidationLayerSupport()
    {
        if (!this->EnableValidationLayers) return true;

        uint32_t layerCount = {};
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : this->ValidationLayers) 
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) 
            {
                if (std::strcmp(layerName, layerProperties.layerName) == 0) 
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) 
                return false;
        }
        return true;
    }

    void VulkanValidationLayer::SetUpDebugMessenger(VkInstance vulkanInstance)
    {
        if(!this->EnableValidationLayers) return;

        if (CreateDebugUtilsMessengerEXT(vulkanInstance, &DebugMessengerCreateInfo, nullptr) != VK_SUCCESS)
            VAL_LAYER("Failed to create debug messenger\n");
    }


    void LogExtensions(const char* extensionsName, const std::vector<VkExtensionProperties>& extensions, const std::vector<const char*> requiredExtensions)
    {
        LOG_MSG("Available ( %s ) extensions:\n      ",extensionsName)
        for (auto& extension : extensions)
        {
            LOG_MSG(extension.extensionName)
            LOG_MSG("\n      ")
        }

        LOG_MSG("\nRequired ( %s ) extensions:\n      ", extensionsName)
        for (auto extension : requiredExtensions)
        {
            LOG_MSG(extension)
            LOG_MSG("\n      ")
        }
        LOG_MSG("\n")
    }

    void vkLayers::VulkanValidationLayer::LogVulkanExtensions(const std::vector<VkExtensionProperties>& extensions, const std::vector<const char*> requiredExtensions)
    {
        if(EnableValidationLayers)
            LogExtensions("Vulkan library Instance", extensions, requiredExtensions);
    }

    void vkLayers::VulkanValidationLayer::LogGpuExtensions(const std::vector<VkExtensionProperties>& extensions, const std::vector<const char*> requiredExtensions)
    {
        if (EnableValidationLayers)
          LogExtensions("Gpu/PhysicalDevice",extensions, requiredExtensions);
    }

}