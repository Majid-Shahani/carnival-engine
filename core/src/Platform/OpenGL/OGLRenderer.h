#pragma once
#include <carnival/Renderer/Renderer.h>
struct GLFWwindow;

namespace Carnival {

	class OpenGLRenderer : public Renderer
	{
	public:
		OpenGLRenderer(GLFWwindow* window);

		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual void SetSwapInterval(bool enabled) override;
	private:
		GLFWwindow* m_WindowHandle;

		void FramebufferResizeCallback() override;
	};
}