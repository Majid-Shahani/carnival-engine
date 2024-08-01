#include "clpch.h"
#include "OpenGLContext.h"
#ifdef CL_OGL
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Carnival {
	OpenGLContext::OpenGLContext(GLFWwindow* window) : m_WindowHandle(window)
	{ CL_CORE_ASSERT(window, "Window Is Null, Context Initialization Failed.") }
	
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CL_CORE_ASSERT(status, "Failed to initialize glad!")
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.5, 0, 0.5, 1);
	}

	void OpenGLContext::SetSwapInterval(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
	}
}
#endif