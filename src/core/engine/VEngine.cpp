#include "VEngine.h"

#include <Windows.h>

#include "core/debugger/public/Logger.h"
#include "core/api/pipelineConfigs/VulkanPipeLineDefaultConfiguration.h"


VEngine::VEngine(const char* appname, HINSTANCE instance)
	: Window( (LPCTSTR)appname )
	, VulkanLib{ nullptr }
	, SwapChain{nullptr}
	, PipelineLayout{nullptr}
	, Pipeline{ nullptr}
	, AppInfo{}
{
	Window.CreateWin32Window(instance);
	VulkanLib = _CreateVulkanInstance(appname);
	SwapChain = new VulkanSwapChain{ *VulkanLib,VkExtent2D{Window.Width,Window.Height} };
	VkExtent2D swapChainExtent = SwapChain->GetSwapChainExtent();
	VulkanPipelineDefaultConfiguration pipelineConfigInfo;
	pipelineConfigInfo.CreatePipelineConfigInfo( swapChainExtent.width, swapChainExtent.height);
	_CreatePipeLineLayout();
	pipelineConfigInfo.PipelineLayout = PipelineLayout;	
	pipelineConfigInfo.Renderpass = SwapChain->GetRenderPass();
	Pipeline = new VulkanPipeline{*VulkanLib, pipelineConfigInfo };
}

VEngine::~VEngine()
{
	delete Pipeline;
	vkDestroyPipelineLayout(VulkanLib->GetVkLogicalDevice(), PipelineLayout, nullptr);
	delete SwapChain;
	// the last thing to be deleted should be the library
	delete VulkanLib;
}

Vulkan* VEngine::_CreateVulkanInstance(const char* appName)
{
	//if throw doesn't matter nothing has been allocated at this moment
	Vulkan* vulkanLib = nullptr;
	vulkanLib = new Vulkan(Window);
	AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	AppInfo.pApplicationName = appName;
	AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.pEngineName = "VEngine";
	AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.apiVersion = VK_API_VERSION_1_0;
	try
	{  // this can throw and if we throw at this moment we have succesfully
	   // allocated vulkanLib pointer so we need to clean up 
		vulkanLib->Init(AppInfo);
	}
	catch (const std::exception& e)
	{
		if (VulkanLib) delete VulkanLib;
		throw e;
	}
	return vulkanLib;
}

void VEngine::_CreatePipeLineLayout()
{
	// This sets uniforms values to shaders can be change at draw time like mvp matrix, texture samples
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	VK_CHECK(vkCreatePipelineLayout(VulkanLib->GetVkLogicalDevice(),&pipelineLayoutInfo,nullptr,&PipelineLayout))
}

void VEngine::_CreateCommandBuffers()
{
}


void VEngine::Run()
{
		MSG message;

		while (GetMessage(&message, NULL, 0, 0))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	

}

void VEngine::Draw()
{
}
