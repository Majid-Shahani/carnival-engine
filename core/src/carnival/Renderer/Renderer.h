#pragma once

namespace Carnival {
	enum class RenderAPI : uint8_t
	{
		NONE = 0,
		OGL = 1, // Open GL
		VULK = 2 // Vulkan
	};

	// Check vulkan Alignment requirements, for mat4s they should be alignas(16)
	// https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap15.html#interfaces-resources-layout
	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	// Remember To change and update GetBindingDescription and GetAttributeDescription in VK impl in case Vertex is Changed
	// TODO: add functions for step and alignment
	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;
	};

	class Renderer {
	public:
		virtual ~Renderer() = default;

		static std::unique_ptr<Renderer> get(RenderAPI api, void* pWindow, bool vSync);

		virtual void clear() = 0;
		virtual void drawFrame() = 0;
		virtual void swapBuffers() = 0;

		virtual void setSwapInterval(bool VSync) = 0;
		virtual void framebufferResizeCallback() = 0;
	};
}