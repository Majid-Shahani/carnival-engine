#pragma once
#include <carnival/Renderer/Renderer.h>

#include "CL_VKDevice.h"
#include "CL_VKPipeline.h"
#include "CL_VKBuffer.h"

namespace Carnival {
	class CL_VKModel {
	public:
		CL_VKModel(CL_VKDevice& device, const std::vector<Carnival::Vertex>& vertices, const std::vector<uint32_t>& indices);
		~CL_VKModel();

        CL_VKModel(const CL_VKModel&) = delete;
        CL_VKModel& operator=(const CL_VKModel&) = delete;
        CL_VKModel(CL_VKModel&&) = delete;
        CL_VKModel& operator=(CL_VKModel&&) = delete;

		void bind(VkCommandBuffer commandBuffer) const;
		void draw(VkCommandBuffer commandBuffer) const;

        // Vertex Struct Related Functions
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }
        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescription() {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, Vertex::pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);
            return attributeDescriptions;
        }

	private:
		CL_VKDevice& m_Device;

        std::unique_ptr<CL_VKBuffer> m_VertexBuffer;
        uint32_t m_VertexCount;

        std::unique_ptr<CL_VKBuffer> m_IndexBuffer;
        uint32_t m_IndexCount;

		void createVertexBuffers(const std::vector<Carnival::Vertex>& vertices);
        void createIndexBuffers(const std::vector<uint32_t>& indices);
	};
}