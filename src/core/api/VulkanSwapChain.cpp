#include "VulkanSwapChain.h"


#include <cassert>
#include "core/debugger/public/Logger.h"
#include "Vulkan.h"


VulkanSwapChain::~VulkanSwapChain()
{
	VkDevice logicalDevice = VulkanLib.GetVkLogicalDevice();
	vkDestroySemaphore(logicalDevice, RenderFinishedSemaphore, nullptr);
	vkDestroySemaphore(logicalDevice, ImageAvailableSemaphore, nullptr);
	vkDestroyRenderPass(logicalDevice, RenderPass, nullptr);

	for (auto imageView : SwapChainImageViews) 
	{
		vkDestroyImageView(logicalDevice, imageView, nullptr);
	}
	vkDestroySwapchainKHR(logicalDevice, SwapChain, nullptr);
}

VulkanSwapChain::VulkanSwapChain(Vulkan& vulkanLib,VkExtent2D windowExtent)
	: VulkanLib{vulkanLib}
	, WindowExtent{ windowExtent }
	, SwapChainExtent{}
	, SwapChain{}
	, SwapChainImageFormat {}
	, DepthImages{}
	, SwapChainImages{}
	, DepthImageViews{}
	, SwapChainImageViews{}
	, RenderPass{}
	, DepthImageMemorys{}
	, FrameBuffers{}
	, ImageAvailableSemaphore{}
	, RenderFinishedSemaphore{}
{
	_CreateSwapChain();
	_CreateImageViews();
	_CreateDepthImageViews();
	_CreateRenderPass();
	_CreateFrameBuffers();
}

void VulkanSwapChain::_CreateSwapChain()
{
	//Check if currentExtent has been set if not we need to set it manually
	VkSurfaceCapabilitiesKHR capabilities = {};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanLib.GetGpu(), VulkanLib.GetSurface(), &capabilities);
	//resolution of images in the swap chain
	SwapChainExtent = (capabilities.currentExtent.width == -1 || capabilities.currentExtent.height == -1)
		? SwapChainExtent = WindowExtent : SwapChainExtent = capabilities.currentExtent;
	// Querry which formats are available in this gpu and choose the appropiate one
	uint32_t formatCount = {};
	vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanLib.GetGpu(), VulkanLib.GetSurface(), &formatCount, nullptr);
	std::vector<VkSurfaceFormatKHR> availableSurfaceFormats(formatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanLib.GetGpu(), VulkanLib.GetSurface(), &formatCount, availableSurfaceFormats.data());
	
	VkSurfaceFormatKHR surfaceFormat = {};
	for (const VkSurfaceFormatKHR& surfaceformat : availableSurfaceFormats)
	{
		if (surfaceformat.format == VK_FORMAT_B8G8R8A8_SRGB 
			&& surfaceformat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			surfaceFormat = surfaceformat;
	}
	SwapChainImageFormat = surfaceFormat.format;

	// Choose the best avaliable presenting mode
	uint32_t presentModeCount = {};
	vkGetPhysicalDeviceSurfacePresentModesKHR(VulkanLib.GetGpu(), VulkanLib.GetSurface(), &presentModeCount,nullptr);
	std::vector<VkPresentModeKHR> availablePresentModes(presentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(VulkanLib.GetGpu(), VulkanLib.GetSurface(), &presentModeCount, availablePresentModes.data());

	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

	for (VkPresentModeKHR mode : availablePresentModes)
	{
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
			presentMode = mode;
	}

	// Image count, triple buffering, double buffering we need to check the minimum supported and maximum
	uint32_t minImageCount = capabilities.maxImageCount > 0 ? capabilities.maxImageCount : 3;

	VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.minImageCount = minImageCount;
	swapChainCreateInfo.surface = VulkanLib.GetSurface();
	swapChainCreateInfo.imageFormat = surfaceFormat.format;
	swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapChainCreateInfo.imageArrayLayers = 1; // always 1 unless develop stereoscopic 3D app
	swapChainCreateInfo.imageExtent = SwapChainExtent;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainCreateInfo.preTransform = capabilities.currentTransform;// we don't need to apply any transform before rendering
	//parameter use indicate if we want to use the alpha channel to blend the window with other windows
	// in the window system. We don't want to 99% of the time
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainCreateInfo.presentMode = presentMode;
	swapChainCreateInfo.clipped = VK_TRUE; // clipp pixels that we don't see e.g: other window in front of our window
															   
	/* If the presentation queue and the graphics queue are not the same then we need
	 to explicitly transfer the image from one queue to another before using it in 
	 another queue faimily. We set default value to EXCLUSIVE if the queues are different 
	 then we modify them
	 */
	swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	uint32_t queueFamilyIndices[] = { VulkanLib.GetGraphicsQueueIndex(), VulkanLib.GetPresentationQueueIndex() };
	if (VulkanLib.GetGraphicsQueueIndex() != VulkanLib.GetPresentationQueueIndex())
	{
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainCreateInfo.queueFamilyIndexCount = 2;
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	// SwapChain can become invalid or unoptimize when the app is running e.g: window resize
	// in that case we will need to recreate from scratch the swap chain and give a reference
	// to the old swapChain
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	VK_CHECK(vkCreateSwapchainKHR(VulkanLib.GetVkLogicalDevice(), &swapChainCreateInfo, nullptr, &SwapChain));

	//get handles to the Images the are allocated with the swap chain and destroy when the swap chain is destroyed
	uint32_t imageCount {0};
	vkGetSwapchainImagesKHR(VulkanLib.GetVkLogicalDevice(),SwapChain,&imageCount,nullptr);
	SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(VulkanLib.GetVkLogicalDevice(), SwapChain, &imageCount, SwapChainImages.data());

}

void VulkanSwapChain::_CreateImageViews()
{
	SwapChainImageViews.resize(SwapChainImages.size());

	for (int i = 0; i < SwapChainImages.size(); ++i)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = SwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = SwapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT ;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.layerCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		if (VK_SUCCESS != vkCreateImageView(VulkanLib.GetVkLogicalDevice(), &createInfo, nullptr, &SwapChainImageViews[i]))
			LOG_ERR("Failed to create image views!")
	}
}

void VulkanSwapChain::_CreateDepthImageViews()
{
	VkFormat depthFormat = VulkanLib.FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	VkExtent2D swapChainExtent = SwapChainExtent;

	DepthImages.resize(SwapChainImages.size());
	DepthImageMemorys.resize(SwapChainImages.size());
	DepthImageViews.resize(SwapChainImages.size());

	for (int i = 0; i < DepthImages.size(); i++)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = swapChainExtent.width;
		imageInfo.extent.height = swapChainExtent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = depthFormat;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.flags = 0;

		VK_CHECK(vkCreateImage(VulkanLib.GetVkLogicalDevice(), &imageInfo, nullptr, &DepthImages[i])," failed to create image!\n");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(VulkanLib.GetVkLogicalDevice(), DepthImages[i], &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;

		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(VulkanLib.GetGpu(), &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((memRequirements.memoryTypeBits & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
				allocInfo.memoryTypeIndex =  i;
				break;
			}
		}

		VK_CHECK(vkAllocateMemory(VulkanLib.GetVkLogicalDevice(), &allocInfo, nullptr, &DepthImageMemorys[i])," failed to allocate image memory!\n");

		VK_CHECK(vkBindImageMemory(VulkanLib.GetVkLogicalDevice(), DepthImages[i], DepthImageMemorys[i], 0), " bind image memory!\n");

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = DepthImages[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = depthFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VK_CHECK(vkCreateImageView(VulkanLib.GetVkLogicalDevice(), &viewInfo, nullptr, &DepthImageViews[i]),"failed to create texture image view!\n");
	}
}


void VulkanSwapChain::_CreateRenderPass()
{
	/*
	   RENDER_PASS :- specify array of attachments and how they will be used eg: color attach, depth,stencil...
	                - specify layout location in which framebuffer attachment to use
			SUBPASS 0 : - references the bind point amost always graphics
			            - attachment counts and types
			.
			.
			.
	   END
	
	*/
	// describe how the attachment will be use in the framebuffer
	//We will use one color attachment
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = SwapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // match multisampling
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;// what to do before rendering
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;// what to do after rendering
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	//We will use one depth/stencil attachment
	VkAttachmentDescription depthStencilAttachment = {};
	depthStencilAttachment.format = VulkanLib.FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT,VK_FORMAT_D32_SFLOAT_S8_UINT,VK_FORMAT_D24_UNORM_S8_UINT }
		, VK_IMAGE_TILING_OPTIMAL
		, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	depthStencilAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // match multisampling
	depthStencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;// what to do before rendering
	depthStencilAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;// what to do after rendering
	depthStencilAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthStencilAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthStencilAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthStencilAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	/*A single render pass can consist of multiple subpasses. Subpasses are subsequent rendering 
	  operations that depend on the contents of framebuffers in previous passes, for example a 
	  sequence of post-processing effects that are applied one after another. If you group these 
	  rendering operations into one render pass, then Vulkan is able to reorder the operations 
	  and conserve memory bandwidth for possibly better performance. For our very first triangle, 
	  however, we'll stick to a single subpass.
	*/
	// It can be multiple color attachments 
	// this specifies layout ( location = <attachment>) out vec4 outColor in the fragment shader
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// It can olny be one depth attachment
	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
	// describe the subpass
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pResolveAttachments = nullptr; // Attachments used for multisampling color attachments
	subpass.pDepthStencilAttachment = &depthAttachmentRef;// depth buffer attachment/stencil
	subpass.pPreserveAttachments = nullptr;// Attachments that are not used by this subpass, but for which the data must be preserved

	VkSubpassDependency dependency = {};

	dependency.dstSubpass = 0;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.srcAccessMask = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	std::vector<VkAttachmentDescription>attachments{ colorAttachment,depthStencilAttachment };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = (uint32_t)attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VK_CHECK(vkCreateRenderPass(VulkanLib.GetVkLogicalDevice(), &renderPassInfo, nullptr, &RenderPass));
}

void VulkanSwapChain::_CreateFrameBuffers()
{
	FrameBuffers.resize(SwapChainImages.size());
	
	for (int i = 0; i < FrameBuffers.size(); ++i)
	{
		VkImageView attachments[2]{SwapChainImageViews[i],DepthImageViews[i]};

		VkFramebufferCreateInfo framebuffCreateInf = {};
		framebuffCreateInf.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffCreateInf.renderPass = RenderPass;
		framebuffCreateInf.attachmentCount = 2;
		framebuffCreateInf.pAttachments = attachments;
		framebuffCreateInf.width = SwapChainExtent.width;
		framebuffCreateInf.height = SwapChainExtent.height;
		framebuffCreateInf.layers = 1;

		VK_CHECK(vkCreateFramebuffer(VulkanLib.GetVkLogicalDevice(), &framebuffCreateInf, nullptr, &FrameBuffers[i]));
	}
}

void VulkanSwapChain::_CreateSyncronizationObjects()
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VK_CHECK(vkCreateSemaphore(VulkanLib.GetVkLogicalDevice(), &semaphoreInfo, nullptr, &ImageAvailableSemaphore));
	VK_CHECK(vkCreateSemaphore(VulkanLib.GetVkLogicalDevice(), &semaphoreInfo, nullptr, &RenderFinishedSemaphore));
}

VkFramebuffer VulkanSwapChain::GetFrameBuffer(int index) const 
{ 
	assert(index < FrameBuffers.size() && index >= 0);
	return FrameBuffers.at(index); 
}

VkResult VulkanSwapChain::AdquireNextImage(uint32_t* index)
{
	return vkAcquireNextImageKHR(VulkanLib.GetVkLogicalDevice(), SwapChain, UINT64_MAX, ImageAvailableSemaphore, VK_NULL_HANDLE, index);
}


