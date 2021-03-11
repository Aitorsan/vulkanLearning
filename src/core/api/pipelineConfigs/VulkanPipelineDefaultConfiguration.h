#ifndef VULKAN_PIPELINE_DEFAULT_CONFIGURATION_HPP
#define VULKAN_PIPELINE_DEFAULT_CONFIGURATION_HPP
#include "IVulkanPipeLineConfiguration.h"

class VulkanPipelineDefaultConfiguration : public IVulkanPipelineConfigurationInfo
{
public:
	DISABLE_COPY_GEN_DEFAULT_CONSTRUCT(VulkanPipelineDefaultConfiguration)
	virtual ~VulkanPipelineDefaultConfiguration() {}
	void CreatePipelineConfigInfo( uint32_t width, uint32_t height) override;

};


#endif //VULKAN_PIPELINE_DEFAULT_CONFIGURATION_HPP