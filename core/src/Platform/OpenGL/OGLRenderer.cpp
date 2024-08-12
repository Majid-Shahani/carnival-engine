#include "clpch.h"
#include "OGLRenderer.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Carnival {
	OpenGLRenderer::OpenGLRenderer(GLFWwindow* window, bool VSync) : m_WindowHandle{ window }, m_VSync(VSync)
	{
		CL_CORE_ASSERT(window, "Window Is Null, Context Initialization Failed.");
	}
	
	void OpenGLRenderer::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CL_CORE_ASSERT(status, "Failed to initialize glad!");
		SetSwapInterval(m_VSync);
	}

	void OpenGLRenderer::DrawFrame()
	{
		glfwSwapBuffers(m_WindowHandle);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.8f, 0.2f, 0.2f, 1);
	}

	void OpenGLRenderer::SetSwapInterval(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
	}
	void OpenGLRenderer::FramebufferResizeCallback()
	{
	}
}