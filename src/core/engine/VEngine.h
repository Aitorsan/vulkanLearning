#ifndef VENGINE_HPP
#define VENGINE_HPP

#include "core/os/Win32Window.h"
#include <vulkan/vulkan.h>
#include <vector>

class VertexBuffer;
class VulkanLib;
class VulkanSwapChain;
class VulkanPipeline;

class VEngine
{
	Win32Window Window;
	VulkanLib* Vulkan;
	VulkanSwapChain* SwapChain;
	VkPipelineLayout_T* PipelineLayout;
	VulkanPipeline* Pipeline;
	VkApplicationInfo AppInfo;
	std::vector<VkCommandBuffer> CommandBuffers;
	VertexBuffer* Vertexbuffer;
	VulkanLib* _CreateVulkanInstance(const char* appName);
	void _CreatePipeLineLayout();
	void _CreateCommandBuffers();
	
public:
	VEngine(const char* appname, HINSTANCE hInstance);
	~VEngine();
	void Run();
	void Draw();
	void RecreateSwapChain();

};

#endif //VENGINE_HPP