#include "clpch.h"

#include "WindowsWindow.h"
#include "carnival/Log.h"

namespace Carnival 
{
	static bool s_GLFWInitialized = false;

	Window* Window::Create(const WindowProperties& props)
	{
		return new WindowsWindow(props);
	}

	void Window::Shutdown()
	{
		glfwTerminate(); // glfw already checks for initialization, no need for a second change
		s_GLFWInitialized = false;
	}


	WindowsWindow::WindowsWindow(const WindowProperties& props) : m_Data(props.Title, props.Width, props.Height)
	{
		CL_CORE_INFO("Creating window {0} ({1} x {2})", props.Title, props.Width, props.Height);
		
		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			CL_CORE_ASSERT(success, "Could not Initialize GLFW");

			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow(props.Width, props.Height, props.Title.c_str(), nullptr, nullptr); // Implicit conversion to int
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);
	}

	WindowsWindow::~WindowsWindow() 
	{
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const { return m_Data.VSync; }
}