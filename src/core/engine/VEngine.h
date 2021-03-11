#ifndef VENGINE_HPP
#define VENGINE_HPP

#include "core/os/Win32Window.h"
#include "core/api/VulkanPipeline.h"
#include "core/api/VulkanLib.h"
#include "core/api/VulkanSwapChain.h"

class VEngine
{
	Win32Window Window;
	VulkanLib* Vulkan;
	VulkanSwapChain* SwapChain;
	VkPipelineLayout_T* PipelineLayout;
	VulkanPipeline* Pipeline;
	VkApplicationInfo AppInfo;
	std::vector<VkCommandBuffer> CommandBuffers;
	VulkanLib* _CreateVulkanInstance(const char* appName);
	void _CreatePipeLineLayout();
	void _CreateCommandBuffers();
	
public:
	VEngine(const char* appname, HINSTANCE hInstance);
	~VEngine();
	void Run();
	void Draw();
};

#endif //VENGINE_HPP