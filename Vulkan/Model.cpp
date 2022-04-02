#include "Model.h"

#include <cassert>

namespace Engine
{
	Model::Model(Device& device, const std::vector<Vertex>& verticies)
		: m_device(device)
	{
		CreateVertexBuffer(verticies);
	}

	Model::~Model()
	{
		vkDestroyBuffer(m_device.GetDevice(), m_vertexBuffer, nullptr);
		vkFreeMemory(m_device.GetDevice(), m_vertexBufferMemory, nullptr);
	}

	void Model::Bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { m_vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	void Model::Draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
	}

	void Model::CreateVertexBuffer(const std::vector<Vertex>& vertices)
	{
		assert(vertices.size() >= 3 && "Model must have at least 3 vertices !");

		m_vertexCount = static_cast<uint32_t>(vertices.size());
		VkDeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;
		m_device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_vertexBuffer, m_vertexBufferMemory);
		void* data;

		vkMapMemory(m_device.GetDevice(), m_vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_device.GetDevice(), m_vertexBufferMemory);
	}

	std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attriuteDescription(2);
		attriuteDescription[0].binding = 0;
		attriuteDescription[0].location = 0;
		attriuteDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
		attriuteDescription[0].offset = offsetof(Vertex, position);

		attriuteDescription[1].binding = 0;
		attriuteDescription[1].location = 1;
		attriuteDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attriuteDescription[1].offset = offsetof(Vertex, color);

		return attriuteDescription;
	}
}
