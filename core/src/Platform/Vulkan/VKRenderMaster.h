#pragma once
#include <carnival/Renderer/Renderer.h>
#include "VKDevice.h"
#include "VKRenderer.h"
#include "VKRenderSystem.h"
#include "VKModel.h"
#include "VKDescriptors.h"

struct GLFWwindow;

namespace Carnival {
	class CL_VKRenderMaster : public Renderer
	{
	public:

		CL_VKRenderMaster(GLFWwindow* window);
		virtual ~CL_VKRenderMaster() override;

		virtual void render() override;
		virtual void loadModel(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) override;
		virtual void framebufferResizeCallback() override { m_Renderer.framebufferResizeCallback(); }
	private:
		void init();
		void updateUniformBuffer(uint32_t frameIndex);

		GLFWwindow* m_Window{ nullptr };
		CL_VKDevice m_Device;
		CL_VKRenderer m_Renderer;
		std::unique_ptr<CL_VKRenderSystem> m_RenderSystem;
		//
		std::vector<CL_VKModel> m_Models{};
		std::vector<std::unique_ptr<CL_VKBuffer>> m_UniformBuffers;

		// any system with a temporary life-time that allocates descriptors using this global pool object,
		// should make sure to free those descriptors using its destructor
		std::unique_ptr<CL_VKDescriptorPoolGrowable> m_GlobalPool{};
		std::unique_ptr<CL_VKDescriptorSetLayout> m_GlobalSetLayout{};
		std::vector<VkDescriptorSet> m_GlobalDescriptorSets{};

	};
}