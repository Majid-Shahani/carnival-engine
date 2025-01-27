#pragma once
/*
#include <carnival/Renderer/Renderer.h>

#include "VKDevice.h"
#include "VKSwapChain.h"
#include "VKPipeline.h"
#include "VKModel.h"
#include "VKDescriptors.h"

struct GLFWwindow;

namespace Carnival {
	class CL_VKRenderer : public Renderer {
	public:

		CL_VKRenderer(GLFWwindow* window, bool VSync = true);
		~CL_VKRenderer();

		void loadModel(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) override;

		void clear() override;
		void drawFrame() override;
		void swapBuffers() override;

		void setSwapInterval(bool VSync) override;
		void framebufferResizeCallback() override;

	private:
		// this is basically a stand-in for data that's supposed to be passed to the class from the outside
		void init();

		void createPipelineLayout(VkDescriptorSetLayout setLayout);
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
		
		void updateUniformBuffer();
		void recordCommandBuffer();

		uint32_t m_CurrentFrame{};
		uint32_t m_ImageIndex{};
		bool m_VSync = true;
		bool m_FramebufferResized = false;
		GLFWwindow* m_Window{ nullptr };
		CL_VKDevice m_Device;
		std::unique_ptr<CL_VKSwapChain> m_SwapChain;
		std::unique_ptr<CL_VKPipeline> m_Pipeline;
		
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		// stuff to be passed in from outside
		std::unique_ptr<CL_VKModel> m_Model;
		std::vector<std::unique_ptr<CL_VKBuffer>> m_UniformBuffers;

		// any system with a temporary life-time that allocates descriptors using this global pool object,
		// should make sure to free those descriptors using its destructor
		std::unique_ptr<CL_VKDescriptorPoolGrowable> m_GlobalPool{};
		std::unique_ptr<CL_VKDescriptorSetLayout> m_GlobalSetLayout{};
		std::vector<VkDescriptorSet> m_GlobalDescriptorSets{};

	};
}
*/