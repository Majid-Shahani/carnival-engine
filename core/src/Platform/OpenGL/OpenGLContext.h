#pragma once
#include <carnival/Renderer/GraphicsContext.h>

struct GLFWwindow;

namespace Carnival {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* window);

		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual void SetSwapInterval(bool enabled) override;
	private:
		GLFWwindow* m_WindowHandle;
	};
}