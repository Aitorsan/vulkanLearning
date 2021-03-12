#ifndef VERTEX_BUFFER_HPP
#define VERTEX_BUFFER_HPP

#include <vector>
#include "core/api/VulkanLib.h"

class VertexBuffer
{

	const VulkanLib& Vulkan;
	VkBuffer VertBuffer;
	VkDeviceMemory VertexBufferDeviceMemory;
	std::vector<float> MeshData;
	std::vector<VkVertexInputBindingDescription> BindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> AttribDescriptions;
	int Stride;
	int Binding;
	
public:

	VertexBuffer(const VulkanLib& vulkan, const std::vector<float> meshData,int stride, int binding, int descriptionCount);
	~VertexBuffer();

	void CreateBuffer();
	void CreateStagingBuffer();
	void BindBuffer(VkCommandBuffer commandBuffer);
	int GetVerticesSize() const { return MeshData.size(); }
	std::vector<VkVertexInputBindingDescription>& GetBindingDescriptions();
	std::vector<VkVertexInputAttributeDescription>& GetAttributeDescriptions();

};

#endif// VERTEX_BUFFER_HPP