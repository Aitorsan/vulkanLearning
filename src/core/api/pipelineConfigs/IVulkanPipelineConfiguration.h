#ifndef VULKAN_PIPELINE_CONFIGURATION_HPP
#define VULKAN_PIPELINE_CONFIGURATION_HPP

#include <vulkan/vulkan.h>


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

	// ColorBlendInfo points to colorBlendAttachment and if we copy this object
	// we might have a dangling pointer, so we need deep copy
	IVulkanPipelineConfigurationInfo() = default;
	IVulkanPipelineConfigurationInfo(const IVulkanPipelineConfigurationInfo&);
	IVulkanPipelineConfigurationInfo& operator=(const IVulkanPipelineConfigurationInfo&);
	virtual ~IVulkanPipelineConfigurationInfo(){}
	virtual void CreatePipelineConfigInfo( uint32_t width, uint32_t height) = 0;
};

#endif //VULKAN_PIPELINE_CONFIGURATION_HPP