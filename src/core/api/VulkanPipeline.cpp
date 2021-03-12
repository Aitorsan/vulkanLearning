#include "VulkanPipeline.h"
#include <cassert>
#include "core/utils/ShaderLoader.h"
#include "core/debugger/public/Logger.h"
#include "core/api/VulkanLib.h"
#include "core/api/pipelineConfigs/IVulkanPipelineConfiguration.h"
#include "core/api/VertexBuffer.h"

VulkanPipeline::VulkanPipeline( const VulkanLib& vulkan, const IVulkanPipelineConfigurationInfo& pipeLineConfigInfo, VertexBuffer* vertexBuffer)
	: Vulkan{vulkan}
	, GraphicsPipeline{nullptr}
	, VertexShaderModule{nullptr}
	, FragShaderModule{nullptr}
{
	ASSERT_NOT_NULL(pipeLineConfigInfo.PipelineLayout, "Pipeline layout missing!\n");
	ASSERT_NOT_NULL(pipeLineConfigInfo.Renderpass, "Render pass config info missing!\n");

	CreateGraphicsPipeline(pipeLineConfigInfo,vertexBuffer);
}

VulkanPipeline::~VulkanPipeline()
{
	vkDestroyPipeline(Vulkan.GetLogicalDevice(), GraphicsPipeline,nullptr);
}


void VulkanPipeline::CreateGraphicsPipeline(const IVulkanPipelineConfigurationInfo& pipeConfig, VertexBuffer* vertexBuffer)
{
	ShaderList shaders = ShaderLoader::loadShaders("glslShaders/vertex.spv", "glslShaders/fragment.spv");
	//TODO: create enums for the indices in shaders vector
	VkShaderModule_T* vertexShaderModule = CreateShaderModule(shaders.at(0));
	VkShaderModule_T* fragmentShaderModule = CreateShaderModule(shaders.at(1));

	// Pipeline Programable shader stages 
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages(shaders.size());
	//VERTEX SHADER
	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = vertexShaderModule;
	shaderStages[0].pName = "main";
	shaderStages[0].pNext = nullptr;
	shaderStages[0].pSpecializationInfo = nullptr;
	//FRAGMENT SHADER
	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = fragmentShaderModule;
	shaderStages[1].pName = "main";
	shaderStages[1].pNext = nullptr;
	shaderStages[1].pSpecializationInfo = nullptr;

	//Describe how to interpret vertex data this is equivalent to glVertexAttribPointer, glEnableVertexAttribArray
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	
	if (vertexBuffer)
	{
		const auto& bindingDescriptions = vertexBuffer->GetBindingDescriptions();
		const auto& attributeDescriptions = vertexBuffer->GetAttributeDescriptions();
		vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
		vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	}
	
	// view port configuration create info
	VkPipelineViewportStateCreateInfo viewportInfo = {};
	viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportInfo.scissorCount = 1;
	viewportInfo.pScissors = &pipeConfig.Scissors;
	viewportInfo.viewportCount = 1;
	viewportInfo.pViewports = &pipeConfig.ViewPort;

	// Configure some dynamic state. We can change this states without recreating the pipeline
	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT,VK_DYNAMIC_STATE_SCISSOR,VK_DYNAMIC_STATE_LINE_WIDTH };
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 3;
	dynamicState.pDynamicStates = dynamicStates;

	// fill int the graphics pipeline information to create the pipeline
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.stageCount = shaderStages.size();//vertex shader and fragment shader for now
	graphicsPipelineCreateInfo.pStages = shaderStages.data();
	graphicsPipelineCreateInfo.pVertexInputState = &vertexInputInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &pipeConfig.InputAssemblyInfo;
	graphicsPipelineCreateInfo.pRasterizationState = &pipeConfig.RasterizerInfo;
	graphicsPipelineCreateInfo.pViewportState = &viewportInfo;
	graphicsPipelineCreateInfo.pColorBlendState = &pipeConfig.ColorBlendInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = &pipeConfig.DepthStencilInfo;
	graphicsPipelineCreateInfo.pMultisampleState = &pipeConfig.MultiSampleInfo;
	graphicsPipelineCreateInfo.pDynamicState = &dynamicState;
	graphicsPipelineCreateInfo.layout = pipeConfig.PipelineLayout;
	graphicsPipelineCreateInfo.renderPass = pipeConfig.Renderpass;
	graphicsPipelineCreateInfo.subpass = pipeConfig.SubPass;
	graphicsPipelineCreateInfo.basePipelineIndex = -1;
	graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

	VK_CHECK(vkCreateGraphicsPipelines(Vulkan.GetLogicalDevice(), nullptr, 1, &graphicsPipelineCreateInfo, nullptr, &GraphicsPipeline));

	//clean shaders
	vkDestroyShaderModule(Vulkan.GetLogicalDevice(), vertexShaderModule, nullptr);
	vkDestroyShaderModule(Vulkan.GetLogicalDevice(), fragmentShaderModule, nullptr);
}

void VulkanPipeline::BindPipeline(VkCommandBuffer_T* cmdBuffer)
{
	vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,GraphicsPipeline );
}


VkShaderModule_T* VulkanPipeline::CreateShaderModule(const std::vector<char>& code  )
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule_T* shaderModule = {nullptr};

	VK_CHECK(vkCreateShaderModule(Vulkan.GetLogicalDevice(), &createInfo, nullptr, &shaderModule));

	return shaderModule;
}



