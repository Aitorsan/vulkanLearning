#include "IVulkanPipelineConfiguration.h"


IVulkanPipelineConfigurationInfo::IVulkanPipelineConfigurationInfo(const IVulkanPipelineConfigurationInfo& other)
	:ViewPort{ other.ViewPort }
	, Scissors{ other.Scissors }
	, InputAssemblyInfo{ other.InputAssemblyInfo }
	, RasterizerInfo{ other.RasterizerInfo }
	, MultiSampleInfo{ other.MultiSampleInfo }
	, ColorBlendAttachment{ other.ColorBlendAttachment }
	, ColorBlendInfo{ other.ColorBlendInfo }
	, DepthStencilInfo{ other.DepthStencilInfo }
	, PipelineLayout{ nullptr}
	, Renderpass{ nullptr }
	, SubPass{ other.SubPass }
{
	// make sure to make pAttachments to point to the new copy
	ColorBlendInfo.pAttachments = &ColorBlendAttachment;
}

IVulkanPipelineConfigurationInfo& IVulkanPipelineConfigurationInfo::operator=(const IVulkanPipelineConfigurationInfo& other)
{
	if (this == &other) return *this;

	this->ViewPort = other.ViewPort;
	this->Scissors = other.Scissors;
	this->InputAssemblyInfo = other.InputAssemblyInfo;
	this->RasterizerInfo = other.RasterizerInfo;
	this->MultiSampleInfo = other.MultiSampleInfo;
	this->ColorBlendAttachment = other.ColorBlendAttachment;
	this->ColorBlendInfo = other.ColorBlendInfo;
	this->DepthStencilInfo = other.DepthStencilInfo;
	// copy data of pipeline layout and renderpass
	this->PipelineLayout = other.PipelineLayout;//This will be deleted by vulkan so more than one object can reference
	this->Renderpass = other.Renderpass;//This will be safetely deleted by vulkan so 2 objects can reference

	this->SubPass = other.SubPass;
	// make sure to make pAttachments to point to the new copy
	this->ColorBlendInfo.pAttachments = &this->ColorBlendAttachment;


}

