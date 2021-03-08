#ifndef VULKAN_PIPELINE_H
#define VULAN_PIPELINE_H
#include <vector>
#include <vulkan/vulkan.h>

class Vulkan;
class IVulkanPipelineConfigurationInfo;

class VulkanPipeline
{
	const Vulkan& VulkanLib;
	VkPipeline GraphicsPipeline;
	VkShaderModule VertexShaderModule;
	VkShaderModule FragShaderModule;
public:
	VulkanPipeline(const Vulkan& vulkan,const IVulkanPipelineConfigurationInfo& pipeLineConfigInfo );
	~VulkanPipeline();

	//not copiable or movable
	VulkanPipeline(const VulkanPipeline&) = delete;
	VulkanPipeline& operator=(const VulkanPipeline&) = delete;

	VkShaderModule_T* CreateShaderModule(const std::vector<char>& code);
	void CreateGraphicsPipeline(const IVulkanPipelineConfigurationInfo& pipeConfig);
	void BindPipeline(VkCommandBuffer_T* cmdBuffer);
};

#endif // VULAN_PIPELINE_H