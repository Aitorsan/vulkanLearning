#include "VulkanPipeLineDefaultConfiguration.h"

VulkanPipelineDefaultConfiguration::VulkanPipelineDefaultConfiguration(const VulkanPipelineDefaultConfiguration& other)
	:IVulkanPipelineConfigurationInfo(other)
{
}

VulkanPipelineDefaultConfiguration& VulkanPipelineDefaultConfiguration::operator=(const VulkanPipelineDefaultConfiguration& other)
{
	IVulkanPipelineConfigurationInfo::operator=(other);
	return *this;
}

void VulkanPipelineDefaultConfiguration::CreatePipelineConfigInfo( uint32_t width, uint32_t height)
{
		//VIEWPORT STATE CONFIGURATION
		// view port maps coordinates (-1,-1) (1,1)
		// to (0,0) left size corner of image
		// and widht, and height of the image
		// if we change the width it will squish the image
		ViewPort.x = 0.0f;
		ViewPort.y = 0.0f;
		ViewPort.width = static_cast<float>(width);
		ViewPort.height = static_cast<float> (height);
		ViewPort.minDepth = 0.0f;
		ViewPort.maxDepth = 1.0f;

		// Scissors is like viewport but instead it cuts the image if does not fit in the scissor extent
		// We use a VkRect2D to set the scissor configuration
		Scissors.offset = { 0,0 };
		Scissors.extent = { width,static_cast<uint32_t>(height) };

		// INPUT ASSEMBLER PIPELINE STAGE
	    InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		//RASTERIZER PIPELINE STAGE
		RasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		RasterizerInfo.depthClampEnable = VK_FALSE;
		RasterizerInfo.rasterizerDiscardEnable = VK_FALSE;// if VK_TRUE the geometry is never rasterized... we definetly want fragments to be rasterizer
		RasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
		RasterizerInfo.lineWidth = 1.0f;
		RasterizerInfo.cullMode = VK_CULL_MODE_NONE; // to discard faces
		RasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;// Which is the winding order of the front face
		RasterizerInfo.depthBiasEnable = VK_FALSE; // controls whether to clamp the fragment’s depth values
		RasterizerInfo.depthBiasConstantFactor = 0.0f; // optional
		RasterizerInfo.depthBiasClamp = 0.0f;// optional
		RasterizerInfo.depthBiasSlopeFactor = 0.0f; // optional

		// This is used to antialiasing MSAA,etc
		MultiSampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		MultiSampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;//Number of samples to use
		MultiSampleInfo.pSampleMask = nullptr; // sample mask test
		MultiSampleInfo.sampleShadingEnable = VK_FALSE;
		MultiSampleInfo.minSampleShading = 1.0f;
		MultiSampleInfo.alphaToCoverageEnable = VK_FALSE;
		MultiSampleInfo.alphaToOneEnable = VK_FALSE;

		// DEPTH/ STENCIL configuration
		DepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		DepthStencilInfo.depthBoundsTestEnable = VK_TRUE;
		DepthStencilInfo.depthWriteEnable = VK_TRUE;
		DepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		DepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		DepthStencilInfo.minDepthBounds = 0.0f;
		DepthStencilInfo.maxDepthBounds = 1.0f;
		DepthStencilInfo.stencilTestEnable = VK_FALSE;
		DepthStencilInfo.front = {};
		DepthStencilInfo.back = {};

		//optional for transparency things
		// Every frame buffer has one of this
		ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
			| VK_COLOR_COMPONENT_G_BIT
			| VK_COLOR_COMPONENT_B_BIT
			| VK_COLOR_COMPONENT_A_BIT;
		ColorBlendAttachment.blendEnable = VK_FALSE;

		//blending options for transparency create info references the array of structures for all of 
		//the framebuffers and allows you to set blend constants that you can use as blend factors 
		ColorBlendInfo = {};
		ColorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		ColorBlendInfo.logicOpEnable = VK_FALSE;
		ColorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
		ColorBlendInfo.attachmentCount = 1;
		ColorBlendInfo.pAttachments = &ColorBlendAttachment;
		ColorBlendInfo.blendConstants[0] = 0.0f; // Optional
		ColorBlendInfo.blendConstants[1] = 0.0f; // Optional
		ColorBlendInfo.blendConstants[2] = 0.0f; // Optional
		ColorBlendInfo.blendConstants[3] = 0.0f;

		
}
