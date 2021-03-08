#pragma once
#include <vector>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include "core/debugger/private/VulkanValidationLayers.h"

class Win32Window;

class Vulkan
{
	VkInstance VulkanInstance;// encapsulates access to Vulkan library on the system
	VkPhysicalDevice PhysicalGpu; // this is the gpu we choose
	VkDevice LogicalDevice;// Logical device is the medium through we comunicate with the physical device

	VkSurfaceKHR WindowSurface;
	VkCommandPool CommandPool;

	int GraphicsQueueIndex;
	VkQueue GraphicsQueue;

	int PresentationQueueIndex;
	VkQueue PresentationQueue;
	
	vkLayers::VulkanValidationLayer ValLayers;
	Win32Window& Window32Api;

	const std::vector<const char*> RequiredGpuDeviceExtensions;// physical device required extensions
    std::vector<const char*> RequiredVkIntanceExtensions;

public:

	Vulkan(Win32Window& window);
	~Vulkan();
	void Init(const VkApplicationInfo& info);
	void CreateVulkanInstance(const VkApplicationInfo& info);

	void SelectPhysicalDevice(VkInstance vulkanInstance, VkSurfaceKHR windowSurface);
	bool GetRequiredQueueFamilyIndices(VkPhysicalDevice physicalGpu, VkSurfaceKHR windowSurface);
	bool HasPhysicalDeviceRequiredExtensionSupport(VkPhysicalDevice gpu);
	bool CheckSwapChainSupport(VkPhysicalDevice gpu,VkSurfaceKHR windowSurface);

	void CreateLogicalDevice( VkPhysicalDevice physicalGpu);
	void CreateQueues(VkDevice logicalDevice);
	void CreateCommandPool(VkDevice logicalDevice);
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkDevice GetVkLogicalDevice() const { return LogicalDevice; }
	VkInstance GetVkInstance()const  { return VulkanInstance; }
	VkPhysicalDevice GetGpu() const{ return PhysicalGpu; }
	VkSurfaceKHR GetSurface()const { return WindowSurface; }
	int GetGraphicsQueueIndex() const { return GraphicsQueueIndex; }
	int GetPresentationQueueIndex() const { return PresentationQueueIndex; }

};

