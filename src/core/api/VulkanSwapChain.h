#ifndef VULKAN_SWAP_CHAIN_HPP
#define VULKAN_SWAP_CHAIN_HPP

#include <vector>
#include <vulkan/vulkan.h>

class Vulkan;

class VulkanSwapChain
{
	Vulkan& VulkanLib;
	VkExtent2D WindowExtent;
	VkExtent2D SwapChainExtent;
	VkSwapchainKHR SwapChain;
	VkFormat SwapChainImageFormat;
	std::vector<VkImage> DepthImages;
	std::vector<VkImage> SwapChainImages;
	std::vector<VkImageView> DepthImageViews;
	std::vector<VkImageView> SwapChainImageViews;
	std::vector<VkDeviceMemory> DepthImageMemorys;
	std::vector<VkFramebuffer> FrameBuffers;
	VkRenderPass RenderPass;
	//Sync objects
	VkSemaphore ImageAvailableSemaphore;
	VkSemaphore RenderFinishedSemaphore;
public:
	~VulkanSwapChain();
	VulkanSwapChain(Vulkan& vulkanLib,VkExtent2D windowExtend);
	VkExtent2D GetSwapChainExtent() const { return SwapChainExtent; }
	VkRenderPass GetRenderPass() const { return RenderPass; }
	VkFramebuffer GetFrameBuffer(int index) const; 
	VkResult AdquireNextImage(uint32_t* index);
	std::size_t ImageCount() const { return SwapChainImages.size(); }

private:
	void _CreateSwapChain();
	void _CreateImageViews();
	void _CreateDepthImageViews();
	// framebuffer attachments that will be used while rendering
	void _CreateRenderPass();
	void _CreateFrameBuffers();
	void _CreateSyncronizationObjects();
};

#endif //VULKAN_SWAP_CHAIN_HPP