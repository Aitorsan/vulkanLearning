#ifndef VULKAN_SWAP_CHAIN_HPP
#define VULKAN_SWAP_CHAIN_HPP

#include <vector>
#include <vulkan/vulkan.h>

class VulkanLib;

class VulkanSwapChain
{
	VulkanLib& Vulkan;
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
	// Allow multiple frames to be in flight but bound the amount of work that piles
	static constexpr int MAX_FRAMES_TO_BE_PROCESSED = 2;
	std::size_t CurrentFrame;
	// GPU-GPU syncronization, each frame needs its own set of semaphores 
	std::vector<VkSemaphore> ImageAvailableSemaphores;
	std::vector<VkSemaphore> RenderFinishedSemaphores;
	//CPU - GPU syncronization  Inflight means currently being rendering/presented to the monitor
	std::vector<VkFence> InFlightFences;
	std::vector<VkFence> ImagesInFlight;
public:
	~VulkanSwapChain();
	VulkanSwapChain(VulkanLib& vulkan,VkExtent2D windowExtend);
	VkExtent2D GetSwapChainExtent() const { return SwapChainExtent; }
	VkRenderPass GetRenderPass() const { return RenderPass; }
	VkFramebuffer GetFrameBuffer(int index) const; 
	VkResult AdquireNextImage(uint32_t* index);
	VkResult SubmitCommandBuffers(const VkCommandBuffer* cmdBuffer, uint32_t* ImageIndex);
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