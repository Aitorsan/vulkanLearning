#define NOMINMAX
#include "VulkanLib.h"
#include <map>
#include <vector>
#include <cstring>
#include <climits>
#include <string>
#include <glm/glm.hpp>
#include "core/os/Win32Window.h"
#include "core/debugger/public/Logger.h"
#undef NOMINMAX;

VulkanLib::VulkanLib(Win32Window& window)
: VulkanInstance {nullptr}
, PhysicalGpu{ VK_NULL_HANDLE }
, LogicalDevice{ nullptr }
, WindowSurface{ nullptr }
, CommandPool{ nullptr }
, GraphicsQueueIndex{-1}
, GraphicsQueue{ nullptr }
, PresentationQueueIndex{-1}
, PresentationQueue{ nullptr }
, ValLayers{}
, Window32Api{ window }
, RequiredGpuDeviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME }
, RequiredVkIntanceExtensions{ VK_KHR_WIN32_SURFACE_EXTENSION_NAME
					, VK_KHR_SURFACE_EXTENSION_NAME }
{
	if (ValLayers.EnableValidationLayers)
		RequiredVkIntanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}

VulkanLib::~VulkanLib()
{
	vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);
	ValLayers.CleanUpValidationLayers(VulkanInstance);
	vkDestroyDevice(LogicalDevice, nullptr);
	vkDestroySurfaceKHR(VulkanInstance,WindowSurface, nullptr);
	vkDestroyInstance(VulkanInstance, NULL);
}


void VulkanLib::Init(const VkApplicationInfo& appInfo)
{
	// Check for validation support if validation layers are enabled
	if (!ValLayers.CheckValidationLayerSupport())
		LOG_ERR("Some requested validation layers have not been found supported\n")

	CreateVulkanInstance(appInfo);

	// If validation Layers are enabled and we have succesfully created a vulkan instance, then we set up Debug messenger
	ValLayers.SetUpDebugMessenger(VulkanInstance);
	Window32Api.CreateWindowSurface(VulkanInstance,&WindowSurface);
	SelectPhysicalDevice(VulkanInstance, WindowSurface);
	CreateLogicalDevice(PhysicalGpu);
	CreateQueues(LogicalDevice);
	CreateCommandPool(LogicalDevice);
}


void VulkanLib::CreateVulkanInstance(const VkApplicationInfo& appInfo)
{
	// check available extensions
	uint32_t extensionCount = {};
	vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableVulkanInstanceExtensions(extensionCount);
	vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, availableVulkanInstanceExtensions.data());
	ValLayers.LogVulkanExtensions(availableVulkanInstanceExtensions, RequiredVkIntanceExtensions);

	// Instance create info
	VkInstanceCreateInfo createInf = {};
	createInf.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInf.pApplicationInfo = &appInfo;
	createInf.enabledExtensionCount = (uint32_t)RequiredVkIntanceExtensions.size();
	createInf.ppEnabledExtensionNames = RequiredVkIntanceExtensions.data();
	
	if (ValLayers.EnableValidationLayers)
	{
		createInf.enabledLayerCount = ValLayers.ValidationLayers.size();
		createInf.ppEnabledLayerNames = ValLayers.ValidationLayers.data();
		createInf.pNext = &ValLayers.DebugMessengerCreateInfo;
	}

	VK_CHECK(vkCreateInstance(&createInf, nullptr, &VulkanInstance), " cant't find a compatilbe Vulkan installable client\n");
}

bool VulkanLib::HasPhysicalDeviceRequiredExtensionSupport(VkPhysicalDevice gpu)
{
	uint32_t extensionCount = {};
	vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, availableExtensions.data());
	ValLayers.LogGpuExtensions( availableExtensions, RequiredGpuDeviceExtensions);
	
	for (const char* deviceExtensionName : RequiredGpuDeviceExtensions)
	{
		bool found = false;
		for (const VkExtensionProperties& extension : availableExtensions)
		{
			if (std::strcmp(extension.extensionName, deviceExtensionName) == 0)
			{
				found = true;
				break;
			}
		}
		if (!found) return false;
	}
	
	return true;
}

bool VulkanLib::CheckSwapChainSupport(VkPhysicalDevice gpu, VkSurfaceKHR windowSurface)
{
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, windowSurface, &formatCount, nullptr);

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, windowSurface, &presentModeCount, nullptr);

	return (formatCount > 0 && presentModeCount > 0);
}


void VulkanLib::SelectPhysicalDevice(VkInstance vulkanInstance, VkSurfaceKHR windowSurface)
{
	uint32_t deviceCount = { 0 };
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, nullptr);

	if (deviceCount == 0) LOG_ERR("Failed to find GPUs with vulkan support!\n");

	std::vector<VkPhysicalDevice> gpuDevices(deviceCount);
	vkEnumeratePhysicalDevices(vulkanInstance, &deviceCount, gpuDevices.data());

	// Score GPUs/devices by features and properties and select the Best one base on score
	std::multimap<float, std::pair<VkPhysicalDevice,std::string>> gpusAvailable = {};
	for (const VkPhysicalDevice& gpu : gpuDevices)
	{
		VkPhysicalDeviceProperties gpuProperties;
		vkGetPhysicalDeviceProperties(gpu,&gpuProperties);

		VkPhysicalDeviceFeatures gpuFeatures;
		vkGetPhysicalDeviceFeatures(gpu, &gpuFeatures);
		
		if (gpuFeatures.geometryShader // we cannot function without geometry shader
			&& GetRequiredQueueFamilyIndices(gpu, windowSurface)// Must support desire queues and surface creation
			&& HasPhysicalDeviceRequiredExtensionSupport(gpu) // Must support the device extensions we required
			&& CheckSwapChainSupport(gpu, windowSurface)// Make sure the gpu has at least one surface format and present modes available
			&& gpuFeatures.samplerAnisotropy)// nice to have, but we want to force it
		{
			// if supports the minimu requirements then we score it based on extra prperties and features
			float score = 0;
			if (gpuProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				score += 1;
			}
			
			//get a value between 0 and 1 based on the max image dimension 2D
			score += (float)gpuProperties.limits.maxImageDimension2D / std::numeric_limits<uint32_t>::max();
			gpusAvailable.insert(
				std::pair<float, std::pair<VkPhysicalDevice,std::string>>
				(score,std::pair<VkPhysicalDevice,std::string>(gpu,gpuProperties.deviceName)));
		}
	}

	LOG_TRACE("Number of gpus available:%d\n", gpusAvailable.size())

	// Select the best gpu available if any was found
	if (!gpusAvailable.empty())
		PhysicalGpu = (--gpusAvailable.end())->second.first;


	if (PhysicalGpu == VK_NULL_HANDLE) LOG_ERR("Failed to find a suitable GPU\n");

	LOG_TRACE("GPU selected:%s\n", (--gpusAvailable.end())->second.second.c_str())
}



void VulkanLib::CreateLogicalDevice(VkPhysicalDevice physicalGpu)
{
	/* Vulkan lets you assign priorities to queues to influence the scheduling 
	   of command buffer execution using floats between 0.0 and 1.0. 
	   This is required even if there is only a single queue */
	float priority = 1.0f;

	// Graphics queue create info
	VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
	graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	graphicsQueueCreateInfo.queueFamilyIndex = GraphicsQueueIndex;
	graphicsQueueCreateInfo.queueCount = 1;
	graphicsQueueCreateInfo.pQueuePriorities = &priority;
	
	// Presentation queue create info
	VkDeviceQueueCreateInfo presentationQueueCreateInfo = {};
	presentationQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	presentationQueueCreateInfo.queueFamilyIndex = PresentationQueueIndex;
	presentationQueueCreateInfo.queueCount = 1;
	presentationQueueCreateInfo.pQueuePriorities = &priority;

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos {graphicsQueueCreateInfo, presentationQueueCreateInfo};

	// Specify the device features the device offer, we are going to use 
	// Note:we based our device selection before in SelectPhysicalDevice
	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.geometryShader = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	// if the presentation queue and graphics queue have same index family set count to 1 and one of the index
	// validation layers will warn that the specification require that a list of index families should be different
	createInfo.queueCreateInfoCount = (GraphicsQueueIndex == PresentationQueueIndex) ? 1 : queueCreateInfos.size();
	createInfo.pQueueCreateInfos = (GraphicsQueueIndex == PresentationQueueIndex) ? &graphicsQueueCreateInfo : queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = RequiredGpuDeviceExtensions.size();
	createInfo.ppEnabledExtensionNames = RequiredGpuDeviceExtensions.data();

	VK_CHECK(vkCreateDevice(physicalGpu, &createInfo, nullptr, &LogicalDevice));
}

void VulkanLib::CreateQueues( VkDevice logicalDevice)
{
	vkGetDeviceQueue(logicalDevice, GraphicsQueueIndex, 0, &GraphicsQueue);
	vkGetDeviceQueue(logicalDevice, PresentationQueueIndex, 0, &PresentationQueue);
}

void VulkanLib::CreateCommandPool(VkDevice logicalDevice)
{
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = GraphicsQueueIndex;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	
	VK_CHECK(vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &CommandPool));
}


bool VulkanLib::GetRequiredQueueFamilyIndices( VkPhysicalDevice physicalGpu, VkSurfaceKHR windowSurface)
{
	uint32_t queueFamilyCount = { 0 };
	vkGetPhysicalDeviceQueueFamilyProperties(physicalGpu, &queueFamilyCount, nullptr);
	
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalGpu, &queueFamilyCount, queueFamilies.data());

	// we just need one queue for graphics and other for presentation
	bool graphicsSupportFound = false;
	bool presentationSupportFound = false;

	// we need to find a graphics queue and presentation queue
	for (int i = 0; i < queueFamilies.size(); ++i)
	{
		VkQueueFamilyProperties queueFamily = queueFamilies[i];

		if (queueFamily.queueCount > 0)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				// found queue family that supports graphics commands
				graphicsSupportFound = true;
				GraphicsQueueIndex = i;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalGpu, i, windowSurface, &presentSupport);
			if ( presentSupport)
			{
				// found a queue family that support presenting images
				presentationSupportFound = true;
				PresentationQueueIndex = i;
			}
		}
		if (graphicsSupportFound && presentationSupportFound)
			break;
	}
	return graphicsSupportFound && presentationSupportFound;
}

VkFormat VulkanLib::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) 
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(PhysicalGpu, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (
			tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}
	LOG_ERR("failed to find supported format!\n")
}