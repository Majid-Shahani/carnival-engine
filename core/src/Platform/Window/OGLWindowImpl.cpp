#include "clpch.h"

#ifdef CL_OGL

#include "WindowImpl.h"

#include "carnival/Event/ApplicationEvent.h"
#include "carnival/Event/KeyEvent.h"
#include "carnival/Event/MouseEvent.h"

namespace Carnival 
{
	static bool s_GLFWInitialized = false;

	// Used in Init() to SetErrorCallback
	static void WindowErrorCallback(int error, const char* description)
	{
		CL_CORE_ERROR("GLFW ERROR ({0}: {1}", error, description);
	}

	Window* Window::Create(const WindowProperties& props)
	{
		return new WindowImpl(props);
	}

	WindowImpl::WindowImpl(const WindowProperties& props) : m_Data(props.Title, props.Width, props.Height)
	{
		CL_CORE_INFO("Creating window {0} ({1} x {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)	Init();

		m_Window = glfwCreateWindow(props.Width, props.Height, props.Title.c_str(), nullptr, nullptr); // Implicit conversion to int
		if (m_Window)
		{
			s_WindowCount++;
			glfwMakeContextCurrent(m_Window);

			int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			CL_CORE_ASSERT(status, "Failed to initialize glad!");

			glfwSetWindowUserPointer(m_Window, &m_Data);
			SetVSync(true);
			SetCallbacks();
		}
		else CL_CORE_ERROR("GLFW Window Creation Failed!");
	}

	WindowImpl::~WindowImpl()
	{
		CL_CORE_INFO("Destroying Window {0}", m_Data.Title);

		glfwDestroyWindow(m_Window);
		s_WindowCount--;
		if (!s_WindowCount)
		{
			CL_CORE_INFO("No Windows Remain, Calling GLFWTerminate");
			glfwTerminate(); // glfw already checks for initialization
			s_GLFWInitialized = false;
		}
	}

	void WindowImpl::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.5, 0, 0.5,1);
	}

	void WindowImpl::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		m_Data.VSync = enabled;
	}


	void WindowImpl::Init()
	{
		CL_CORE_INFO("Initializing GLFW");
		int success = glfwInit();
		CL_CORE_ASSERT(success, "Could not Initialize GLFW");
		if (success) s_GLFWInitialized = true;
		glfwSetErrorCallback(WindowErrorCallback);
	}

	void WindowImpl::SetCallbacks()
	{
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent ev(width, height);
				data.EventCallback(ev);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		// Convert Keycodes to engine specific rather than platform specific
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.EventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 1);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event(xOffset, yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMovedEvent event(xPos, yPos);
				data.EventCallback(event);
			});
	}
}

#endif