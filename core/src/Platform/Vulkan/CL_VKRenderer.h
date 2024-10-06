#pragma once
#include <carnival/Renderer/Renderer.h>
#include <carnival/Window.h>

#include "CL_VKDevice.h"
#include "CL_VKSwapChain.h"
#include "CL_VKPipeline.h"
#include "CL_VKModel.h"
#include "CL_VKDescriptors.h"

struct GLFWwindow {};

namespace Carnival {
	class CL_VKRenderer : public Renderer {
	public:

		CL_VKRenderer(GLFWwindow* window, bool VSync = true);
		~CL_VKRenderer();

		// this is basically a stand-in for data that's supposed to be passed to the class from the outside
		void init();
		void drawFrame() override;
		void setSwapInterval(bool VSync) override;
		void framebufferResizeCallback() override;
	private:
		void loadModels();

		void createPipelineLayout(VkDescriptorSetLayout setLayout);
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
		
		void updateUniformBuffer(uint32_t currentFrame);
		void recordCommandBuffer(uint32_t imageindex);

		uint32_t m_CurrentFrame{};
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
		std::unique_ptr<CL_VKDescriptorPool> m_GlobalPool{};
		// any system with a temporary life-time that allocates descriptors using this pool object,
		// should make sure to free those descriptors using its destructor
		std::unique_ptr<CL_VKDescriptorSetLayout> m_GlobalSetLayout{};
		std::vector<VkDescriptorSet> m_GlobalDescriptorSets;
	};
}