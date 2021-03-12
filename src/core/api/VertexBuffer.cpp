#include "VertexBuffer.h"
#include "core/debugger/public/Logger.h"


VertexBuffer::VertexBuffer(const VulkanLib& vulkan,const std::vector<float> meshData, int stride,int binding, int descriptions)
	: Vulkan{vulkan}
	, VertBuffer{}
	, VertexBufferDeviceMemory{}
	, MeshData{meshData}
	, BindingDescriptions{}
	, AttribDescriptions{}
	, Stride{stride}
	, Binding{binding}
{

	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = Binding;
	bindingDescription.stride = Stride;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription position = {};
	VkVertexInputAttributeDescription color = {};

	position.binding = Binding;
	position.location = 0;
	position.format = VK_FORMAT_R32G32B32_SFLOAT;
	position.offset = 0;

	color.binding = Binding;
	color.location = 1;
	color.format = VK_FORMAT_R32G32B32_SFLOAT;
	 color.offset = 3* sizeof(float);


	BindingDescriptions.push_back(bindingDescription);
	AttribDescriptions.push_back(position);
	AttribDescriptions.push_back(color);
	CreateBuffer();
}

VertexBuffer::~VertexBuffer()
{
	vkDestroyBuffer(Vulkan.GetLogicalDevice(), VertBuffer, nullptr);
	vkFreeMemory(Vulkan.GetLogicalDevice(), VertexBufferDeviceMemory, nullptr);
}


void VertexBuffer::CreateBuffer()
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(MeshData[0]) * MeshData.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.flags = 0;

	VK_CHECK(vkCreateBuffer(Vulkan.GetLogicalDevice(), &bufferInfo, nullptr, &VertBuffer));

	//Allocate memory
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(Vulkan.GetLogicalDevice(), VertBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;

	// Set memory type index
	VkPhysicalDeviceMemoryProperties memProperties = {};
	vkGetPhysicalDeviceMemoryProperties(Vulkan.GetGpu(), &memProperties);
	VkMemoryPropertyFlags propertiesWantedFlags{ VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
	
	for (int i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		if ((memRequirements.memoryTypeBits & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & propertiesWantedFlags))
		{
			allocInfo.memoryTypeIndex = i;
			break;
		}
	}

	VK_CHECK(vkAllocateMemory(Vulkan.GetLogicalDevice(), &allocInfo, nullptr, &VertexBufferDeviceMemory));

	vkBindBufferMemory(Vulkan.GetLogicalDevice(), VertBuffer, VertexBufferDeviceMemory,0);

	void* data{ nullptr };
	vkMapMemory(Vulkan.GetLogicalDevice(), VertexBufferDeviceMemory, 0, bufferInfo.size, 0, &data);
	std::memcpy(data, MeshData.data(), (std::size_t)bufferInfo.size);
	vkUnmapMemory(Vulkan.GetLogicalDevice(), VertexBufferDeviceMemory);
}

void VertexBuffer::CreateStagingBuffer()
{

}

std::vector<VkVertexInputBindingDescription>& VertexBuffer::GetBindingDescriptions()
{
	return BindingDescriptions;
}

void VertexBuffer::BindBuffer(VkCommandBuffer commandBuffer)
{
	constexpr VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &VertBuffer, offsets);
}

std::vector<VkVertexInputAttributeDescription>& VertexBuffer::GetAttributeDescriptions()
{
	return AttribDescriptions;

}