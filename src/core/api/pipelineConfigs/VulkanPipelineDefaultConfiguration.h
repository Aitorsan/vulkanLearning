#ifndef VULKAN_PIPELINE_DEFAULT_CONFIGURATION
#define VULKAN_PIPELINE_DEFAULT_CONFIGURATION
#include "IVulkanPipeLineConfiguration.h"

class VulkanPipelineDefaultConfiguration : public IVulkanPipelineConfigurationInfo
{
public:
	DISABLE_COPY_GEN_DEFAULT_CONSTRUCTOR(VulkanPipelineDefaultConfiguration)
	virtual ~VulkanPipelineDefaultConfiguration() {}
	void CreatePipelineConfigInfo( uint32_t width, uint32_t height) override;

};


#endif //VULKAN_PIPELINE_DEFAULT_CONFIGURATION