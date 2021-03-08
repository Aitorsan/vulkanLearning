#ifndef VULKAN_PIPELINE_CONFIGURATION_HPP
#define VULKAN_PIPELINE_CONFIGURATION_HPP

#include <vulkan/vulkan.h>
#include "defines.h"

class IVulkanPipelineConfigurationInfo
{
public:
	VkViewport ViewPort = {};
	VkRect2D Scissors = {};
	VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo = {};
	VkPipelineRasterizationStateCreateInfo RasterizerInfo = {};
	VkPipelineMultisampleStateCreateInfo MultiSampleInfo = {};
	VkPipelineColorBlendAttachmentState ColorBlendAttachment = {};
	VkPipelineColorBlendStateCreateInfo ColorBlendInfo = {};
	VkPipelineDepthStencilStateCreateInfo DepthStencilInfo = {};
	VkPipelineLayout_T* PipelineLayout = nullptr;
	VkRenderPass_T* Renderpass = nullptr;
	uint32_t SubPass = 0;

	DISABLE_COPY_GEN_DEFAULT_CONSTRUCTOR(IVulkanPipelineConfigurationInfo)
		
	virtual ~IVulkanPipelineConfigurationInfo(){}
	virtual void CreatePipelineConfigInfo( uint32_t width, uint32_t height) = 0;
};

#endif //VULKAN_PIPELINE_CONFIGURATION_HPP