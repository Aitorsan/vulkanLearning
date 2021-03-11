#ifndef VULKAN_PIPELINE_H
#define VULAN_PIPELINE_H
#include <vector>
#include <vulkan/vulkan.h>
#include "defines.h"

class VulkanLib;
class IVulkanPipelineConfigurationInfo;

class VulkanPipeline
{
	const VulkanLib& Vulkan;
	VkPipeline GraphicsPipeline;
	VkShaderModule VertexShaderModule;
	VkShaderModule FragShaderModule;
public:
	
	DISABLE_COPY( VulkanPipeline)

	VulkanPipeline(const VulkanLib& vulkan,const IVulkanPipelineConfigurationInfo& pipeLineConfigInfo );
	~VulkanPipeline();
	VkShaderModule_T* CreateShaderModule(const std::vector<char>& code);
	void CreateGraphicsPipeline(const IVulkanPipelineConfigurationInfo& pipeConfig);
	void BindPipeline(VkCommandBuffer_T* cmdBuffer);
};

#endif // VULAN_PIPELINE_H