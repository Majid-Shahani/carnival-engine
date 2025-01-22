#pragma once
#include <carnival/Renderer/Renderer.h>
struct GLFWwindow;

namespace Carnival {

	class OpenGLRenderer : public Renderer
	{
	public:
		OpenGLRenderer(GLFWwindow* window, bool VSync = true);
		virtual ~OpenGLRenderer();

		virtual void clear() override;
		virtual void loadModel(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) override;
		virtual void drawFrame() override;
		virtual void swapBuffers() override;
		virtual void setSwapInterval(bool enabled) override;
		virtual void framebufferResizeCallback() override;
	private:
		GLFWwindow* m_WindowHandle;
		bool m_VSync;
		uint32_t m_VertexArray{}, m_VertexBuffer{}, m_IndexBuffer{};
	};
}