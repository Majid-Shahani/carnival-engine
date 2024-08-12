#pragma once
#include <carnival/Renderer/Renderer.h>
struct GLFWwindow;

namespace Carnival {

	class OpenGLRenderer : public Renderer
	{
	public:
		OpenGLRenderer(GLFWwindow* window, bool VSync = true);
		virtual ~OpenGLRenderer() {}

		virtual void Init() override;
		virtual void DrawFrame() override;
		virtual void SetSwapInterval(bool enabled) override;
	private:
		GLFWwindow* m_WindowHandle;
		bool m_VSync;

		void FramebufferResizeCallback() override;
	};
}