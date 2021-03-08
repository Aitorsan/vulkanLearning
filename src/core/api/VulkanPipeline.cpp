#include "VulkanPipeline.h"
#include <cassert>
#include "core/utils/ShaderLoader.h"
#include "core/debugger/public/Logger.h"
#include "core/api/Vulkan.h"
#include "core/api/pipelineConfigs/IVulkanPipelineConfiguration.h"


VulkanPipeline::VulkanPipeline( const Vulkan& vulkanlib, const IVulkanPipelineConfigurationInfo& pipeLineConfigInfo)
	: VulkanLib{vulkanlib}
	, GraphicsPipeline{nullptr}
	, VertexShaderModule{nullptr}
	, FragShaderModule{nullptr}
{
	ASSERT_NOT_NULL(pipeLineConfigInfo.PipelineLayout, "Pipeline layout missing!\n");
	ASSERT_NOT_NULL(pipeLineConfigInfo.Renderpass, "Render pass config info missing!\n");

	CreateGraphicsPipeline(pipeLineConfigInfo);
}

VulkanPipeline::~VulkanPipeline()
{
	vkDestroyPipeline(VulkanLib.GetVkLogicalDevice(), GraphicsPipeline,nullptr);
}

void VulkanPipeline::CreateGraphicsPipeline(const IVulkanPipelineConfigurationInfo& pipeConfig)
{
	ShaderList shaders = ShaderLoader::loadShaders("glslShaders/vertex.spv", "glslShaders/fragment.spv");

	VkShaderModule_T* vertexShaderModule = CreateShaderModule(shaders.at(0));//TODO: create enums for the indices in shaders vector
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
	vertexInputInfo.pVertexAttributeDescriptions = nullptr;//glVertexAttribPointer
	vertexInputInfo.pVertexBindingDescriptions = nullptr;//glEnableVertexAttribArray
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	
	// view port configuration create info
	VkPipelineViewportStateCreateInfo viewportInfo = {};
	viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportInfo.scissorCount = 1;
	viewportInfo.pScissors = &pipeConfig.Scissors;
	viewportInfo.viewportCount = 1;
	viewportInfo.pViewports = &pipeConfig.ViewPort;

	// Configure some dynamic state. We can change this states without recreating the pipeline
	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT,VK_DYNAMIC_STATE_LINE_WIDTH };
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
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

	if (VK_SUCCESS != vkCreateGraphicsPipelines(VulkanLib.GetVkLogicalDevice(), nullptr, 1, &graphicsPipelineCreateInfo, nullptr, &GraphicsPipeline))
		LOG_ERR("Failed to create graphics Pipeline")

	//clean shaders
	vkDestroyShaderModule(VulkanLib.GetVkLogicalDevice(), vertexShaderModule, nullptr);
	vkDestroyShaderModule(VulkanLib.GetVkLogicalDevice(), fragmentShaderModule, nullptr);
}


VkShaderModule_T* VulkanPipeline::CreateShaderModule(const std::vector<char>& code  )
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule_T* shaderModule = {nullptr};

	if (vkCreateShaderModule(VulkanLib.GetVkLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		LOG_ERR("Failed to create Shader module")

	return shaderModule;
}


