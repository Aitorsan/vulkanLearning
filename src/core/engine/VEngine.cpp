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
	, CommandBuffers{}
{
	Window.CreateWin32Window(instance);
	VulkanLib = _CreateVulkanInstance(appname);
	SwapChain = new VulkanSwapChain{ *VulkanLib,VkExtent2D{Window.Width,Window.Height} };
	VkExtent2D swapChainExtent = SwapChain->GetSwapChainExtent();
	VulkanPipelineDefaultConfiguration pipelineConfigInfo;
	pipelineConfigInfo.CreatePipelineConfigInfo(swapChainExtent.width, swapChainExtent.height);
	
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

	VK_CHECK(vkCreatePipelineLayout(VulkanLib->GetVkLogicalDevice(), &pipelineLayoutInfo, nullptr, &PipelineLayout));
}

void VEngine::_CreateCommandBuffers()
{
	CommandBuffers.resize(SwapChain->ImageCount());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo. commandPool = VulkanLib->GetCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;// can be submit for exec but not being called by other commands
	allocInfo.commandBufferCount = (uint32_t) CommandBuffers.size();

	VK_CHECK(vkAllocateCommandBuffers(VulkanLib->GetVkLogicalDevice(), &allocInfo, CommandBuffers.data()));

	// we associate each command buffer with one of the framebuffers available
	for (int i = 0; i < CommandBuffers.size(); ++i)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		//start comman buffer recording
		VK_CHECK(vkBeginCommandBuffer(CommandBuffers[i], &beginInfo));
		
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = SwapChain->GetRenderPass();
		renderPassInfo.framebuffer = SwapChain->GetFrameBuffer(i);
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = SwapChain->GetSwapChainExtent();

		VkClearValue clearValues[2] = {};
		clearValues[0].color = VkClearColorValue{ 0.0f,0.0f,0.0f,1.0f };
		clearValues[1].depthStencil = VkClearDepthStencilValue{ 1.0f,0 };
		
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues;

		//Start render pass
		vkCmdBeginRenderPass(CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		// bind graphics pipeline
		Pipeline->BindPipeline(CommandBuffers[i]);
		// set the draw command
		vkCmdDraw(CommandBuffers[i], 3, 1, 0, 0);
		//End render pass
		vkCmdEndRenderPass(CommandBuffers[i]);

		//end command buffer recording
		VK_CHECK(vkEndCommandBuffer(CommandBuffers[i]));
	}
}


void VEngine::Run()
{
		MSG message;

		while (GetMessage(&message, NULL, 0, 0))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	    // wait until all gpu operations have been executed like command buffer execution
		vkDeviceWaitIdle(VulkanLib->GetVkLogicalDevice());
}

void VEngine::Draw()
{
	// Adquire image from the swapchain
	//SwapChain->AdquireNextImage(index);
	// Exectue the command buffer with that image attached in the framebuffer
	
	// Return the image to the swapchain
}
