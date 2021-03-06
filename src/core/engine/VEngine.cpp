#include "VEngine.h"

#include <Windows.h>

#include "core/debugger/public/Logger.h"
#include "core/api/pipelineConfigs/VulkanPipeLineDefaultConfiguration.h"
#include "core/api/VulkanSwapChain.h"
#include "core/api/VulkanPipeline.h"
#include "core/api/VulkanLib.h"
#include "core/api/VertexBuffer.h"

VEngine::VEngine(const char* appname, HINSTANCE instance)
	: Window( (LPCTSTR)appname )
	, Vulkan{ nullptr }
	, SwapChain{nullptr}
	, PipelineLayout{nullptr}
	, Pipeline{ nullptr}
	, AppInfo{}
	, CommandBuffers{}
	, Vertexbuffer{nullptr}
{
    
	Window.CreateWin32Window(instance);
	Vulkan = _CreateVulkanInstance(appname);
	SwapChain = new VulkanSwapChain{ *Vulkan,VkExtent2D{Window.Width,Window.Height} };
	VkExtent2D swapChainExtent = SwapChain->GetSwapChainExtent();
	_CreatePipeLineLayout();
	VulkanPipelineDefaultConfiguration pipelineConfigInfo;
	pipelineConfigInfo.CreatePipelineConfigInfo(swapChainExtent.width, swapChainExtent.height);
	
	pipelineConfigInfo.PipelineLayout = PipelineLayout;	
	pipelineConfigInfo.Renderpass = SwapChain->GetRenderPass();

	std::vector<float> triangle
	{
		0.0f,-0.5f,0.0,  1.0f,0.0f,0.0f,
		0.5f,0.5f,0.0,   0.0f,1.0f,0.0f,
		-0.5f,0.5f,0.0,  0.0f,0.0f,1.0f
	};

	Vertexbuffer = new VertexBuffer(*Vulkan, triangle, 6* sizeof(float), 0, 2);
	Pipeline = new VulkanPipeline{*Vulkan, pipelineConfigInfo,Vertexbuffer};
	_CreateCommandBuffers();


}

VEngine::~VEngine()
{
	delete Vertexbuffer;
	vkFreeCommandBuffers(Vulkan->GetLogicalDevice(), Vulkan->GetCommandPool(), static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
	delete Pipeline;
	vkDestroyPipelineLayout(Vulkan->GetLogicalDevice(), PipelineLayout, nullptr);
	delete SwapChain;
	// the last thing to be deleted should be the library
	delete Vulkan;
}

VulkanLib* VEngine::_CreateVulkanInstance(const char* appName)
{
	//if throw doesn't matter nothing has been allocated at this moment
	VulkanLib* vulkan = nullptr;
	vulkan = new VulkanLib(Window);
	AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	AppInfo.pApplicationName = appName;
	AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.pEngineName = "VEngine";
	AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.apiVersion = VK_API_VERSION_1_0;
	try
	{  // this can throw and if we throw at this moment we have succesfully
	   // allocated vulkanLib pointer so we need to clean up 
		vulkan->Init(AppInfo);
	}
	catch (const std::exception& e)
	{
		if (vulkan) delete vulkan;
		vulkan = nullptr;
		throw e;
	}
	return vulkan;
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


	VK_CHECK(vkCreatePipelineLayout(Vulkan->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &PipelineLayout));
}

void VEngine::_CreateCommandBuffers()
{
	CommandBuffers.resize(SwapChain->ImageCount());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo. commandPool = Vulkan->GetCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;// can be submit for exec but not being called by other commands
	allocInfo.commandBufferCount = (uint32_t) CommandBuffers.size();
	
	VK_CHECK(vkAllocateCommandBuffers(Vulkan->GetLogicalDevice(), &allocInfo, CommandBuffers.data()));

	// we associate each command buffer with one of the framebuffers available
	for (int i = 0; i < CommandBuffers.size(); ++i)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;	
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		//start comman buffer recording
		VK_CHECK(vkBeginCommandBuffer(CommandBuffers[i], &beginInfo));
		
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = SwapChain->GetRenderPass();
		renderPassInfo.framebuffer = SwapChain->GetFrameBuffer(i);
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = SwapChain->GetSwapChainExtent();
		
		VkClearValue clearValues[2] = {};
		clearValues[0].color = VkClearColorValue{ 0.5f,0.3f,0.8f,1.0f };
		clearValues[1].depthStencil = VkClearDepthStencilValue{ 1.0f,0 };
		
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues;

		//Start render pass
		vkCmdBeginRenderPass(CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		// bind graphics pipeline 
		// Binding a pipeline is very similar to glUseProgram, 
		// with much more state than only the programmable shaders
		Pipeline->BindPipeline(CommandBuffers[i]);

		// set the view port and scissors dynamically so we don't have to recreate the pipeline
		VkExtent2D  extent = SwapChain->GetSwapChainExtent();
		VkViewport viewport = {};
		viewport.height = (float)extent.height;
		viewport.width = (float)extent.width;
		viewport.minDepth = (float)0.0f;
		viewport.maxDepth = (float)1.0f;
		viewport.x = 0;
		viewport.y = 0;
		vkCmdSetViewport(CommandBuffers[i], 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = { 0,0 };
		scissor.extent = { extent.width,(uint32_t)extent.height };
		vkCmdSetScissor(CommandBuffers[i], 0, 1, &scissor);

        // bind vertex buffer
		Vertexbuffer->BindBuffer(CommandBuffers[i]);
		// set the draw command
		vkCmdDraw(CommandBuffers[i], (uint32_t)Vertexbuffer->GetVerticesSize() , 1, 0, 0);
		//End render pass
		vkCmdEndRenderPass(CommandBuffers[i]);

		//end command buffer recording
		VK_CHECK(vkEndCommandBuffer(CommandBuffers[i]));
	}
}


void VEngine::Run()
{
	while (!Window.ShouldClose())
	{
       
		Draw();
		Window.PoolEvents();
	}
	// wait until all gpu operations have been executed like command buffer execution
    vkDeviceWaitIdle(Vulkan->GetLogicalDevice());
}
void VEngine::RecreateSwapChain()
{
	vkDeviceWaitIdle(Vulkan->GetLogicalDevice());
	vkFreeCommandBuffers(Vulkan->GetLogicalDevice(), Vulkan->GetCommandPool(), static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
	SwapChain->CleanupSwapChain();
	SwapChain->RecreateSwapChain();
	_CreateCommandBuffers();
}

void VEngine::Draw()
{
	// Adquire image from the swapchain
	uint32_t index;
	VkResult result = SwapChain->AdquireNextImage(&index);
	
	if (result == VK_ERROR_OUT_OF_DATE_KHR) 
	{
		vkDeviceWaitIdle(Vulkan->GetLogicalDevice());
		vkFreeCommandBuffers(Vulkan->GetLogicalDevice(), Vulkan->GetCommandPool(), static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
		SwapChain->CleanupSwapChain();
		SwapChain->RecreateSwapChain();
		_CreateCommandBuffers();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
	{
		LOG_ERR("failed to acquire swap chain image!");
	}
	

	// Submit the command buffer for execution with that image attached in the framebuffer
	result = SwapChain->SubmitCommandBuffers(&CommandBuffers[index], &index);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		vkDeviceWaitIdle(Vulkan->GetLogicalDevice());

		vkFreeCommandBuffers(Vulkan->GetLogicalDevice(), Vulkan->GetCommandPool(), static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
		SwapChain->CleanupSwapChain();
		SwapChain->RecreateSwapChain();
		_CreateCommandBuffers();
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
	{
		LOG_ERR("failed to acquire swap chain image!");
	}

}
