#include <clpch.h>
#include "VKRenderMaster.h"
#include "VKBuffer.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Carnival {

	CL_VKRenderMaster::CL_VKRenderMaster(GLFWwindow* window) 
		: m_Window{ window }, m_Device{ window }, m_Renderer{ window, m_Device }
	{
		init();
		m_RenderSystem = 
			std::make_unique<CL_VKRenderSystem>(
				m_Device,
				m_Renderer.getSwapChainRenderPass(),
				m_Renderer.getSwapChainExtent(),
				m_GlobalSetLayout->getLayout());
	}

	CL_VKRenderMaster::~CL_VKRenderMaster()
	{
		vkDeviceWaitIdle(m_Device.device());
		// Destroy members in order
		m_GlobalPool.reset();
	}

	void CL_VKRenderMaster::init() {
		// Mostly UBO and Descriptors
		// Make uniform buffers
		m_UniformBuffers.resize(CL_VKSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < m_UniformBuffers.size(); i++) {
			m_UniformBuffers[i] = std::make_unique<CL_VKBuffer>(
				m_Device,
				sizeof(Carnival::UniformBufferObject),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_Device.m_PhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
			m_UniformBuffers[i]->map();
		}
		
		// Make Descriptor Pools and Descriptor Sets
		m_GlobalSetLayout = std::make_unique<CL_VKDescriptorSetLayout>();
		m_GlobalSetLayout->addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
		m_GlobalSetLayout->build(m_Device.device());

		std::vector<VkDescriptorPoolSize> poolSize;
		poolSize.emplace_back(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, CL_VKSwapChain::MAX_FRAMES_IN_FLIGHT);
		m_GlobalPool = std::make_unique<CL_VKDescriptorPoolGrowable>(
			m_Device,
			CL_VKSwapChain::MAX_FRAMES_IN_FLIGHT,
			0,
			poolSize);

		m_GlobalDescriptorSets.resize(CL_VKSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < m_GlobalDescriptorSets.size(); i++)
		{
			auto bufferInfo = m_UniformBuffers[i]->descriptorInfo();
			CL_VKDescriptorWriter(*m_GlobalSetLayout, *m_GlobalPool)
				.writeBuffer(0, &bufferInfo)
				.build(m_GlobalDescriptorSets[i]);
		}
	}
	void CL_VKRenderMaster::updateUniformBuffer(uint32_t frameIndex)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		VkExtent2D extent = m_Renderer.getSwapChainExtent();

		Carnival::UniformBufferObject ubo{
			.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			//+ glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
			.view = glm::lookAt(glm::vec3(0.0f, -1.0f, 1.0f), glm::vec3{}, glm::vec3{0.0f, 1.0f, 0.0f}),
			.proj = glm::perspective(glm::radians(75.0f), (float)extent.width / (float)extent.height, 0.1f, 10.0f)
		};
		ubo.proj[1][1] *= -1; // do this for vulkan, not for OGL
		m_UniformBuffers[frameIndex]->writeToBuffer(&ubo);
	}

	void CL_VKRenderMaster::render()
	{
		if (VkCommandBuffer cb = m_Renderer.beginFrame())
		{
			uint32_t frame = m_Renderer.getFrameIndex();
			updateUniformBuffer(frame);

			m_Renderer.beginSwapChainRenderPass(cb);
			m_RenderSystem->renderGameObjects(cb, m_Models, m_GlobalDescriptorSets[frame]);
			m_Renderer.endSwapChainRenderPass(cb);

			m_Renderer.endFrame();
		}
	}

	void CL_VKRenderMaster::loadModel(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		m_Models.emplace_back(m_Device, vertices, indices);
	}
}
