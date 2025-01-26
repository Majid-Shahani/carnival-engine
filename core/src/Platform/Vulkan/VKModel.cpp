#include <clpch.h>
#include "VKModel.h"

namespace Carnival {
	CL_VKModel::CL_VKModel(CL_VKDevice& device, const std::vector<Carnival::Vertex>& vertices, const std::vector<uint32_t>& indices)
		:m_Device{ device }
	{
		createVertexBuffers(vertices);
		createIndexBuffers(indices);
	}

	CL_VKModel::~CL_VKModel() {}

	void CL_VKModel::draw(VkCommandBuffer commandBuffer) const
	{
		vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
	}

	void CL_VKModel::bind(VkCommandBuffer commandBuffer) const
	{
		VkBuffer vertexBuffers[] = { m_VertexBuffer->getBuffer()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	void CL_VKModel::createVertexBuffers(const std::vector<Carnival::Vertex>& vertices)
	{
		m_VertexCount = static_cast<uint32_t>(vertices.size());
		CL_CORE_ASSERT((m_VertexCount > 2), "Vertex Count must be at least 3!");

		VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		CL_VKBuffer stagingBuffer{
			m_Device,
			vertexSize,
			m_VertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		stagingBuffer.map(); // unmapped automatically as stagingBuffer goes out of scope
		stagingBuffer.writeToBuffer((void*)vertices.data());

		m_VertexBuffer = std::make_unique<CL_VKBuffer>(
			m_Device,
			vertexSize,
			m_VertexCount,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_Device.CopyBuffer(stagingBuffer.getBuffer(), m_VertexBuffer->getBuffer(), m_VertexBuffer->getBufferSize());
	}
	void CL_VKModel::createIndexBuffers(const std::vector<uint32_t>& indices)
	{
		m_IndexCount = static_cast<uint32_t>(indices.size());
		CL_CORE_ASSERT((m_IndexCount > 2), "Index Count must be at least 3!");

		VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;
		uint32_t indexSize = sizeof(indices[0]);

		CL_VKBuffer stagingBuffer{
			m_Device,
			indexSize,
			m_IndexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

		stagingBuffer.map(); // unmapped automatically as stagingBuffer goes out of scope
		stagingBuffer.writeToBuffer((void*)indices.data());

		m_IndexBuffer = std::make_unique<CL_VKBuffer>(
			m_Device,
			indexSize,
			m_IndexCount,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		m_Device.CopyBuffer(stagingBuffer.getBuffer(), m_IndexBuffer->getBuffer(), m_IndexBuffer->getBufferSize());
	}

}