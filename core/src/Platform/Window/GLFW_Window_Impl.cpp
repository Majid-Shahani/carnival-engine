#include "clpch.h"

#include "WindowImpl.h"

#include <GLFW/glfw3.h>
#include <Platform/OpenGL/OGLRenderer.h>
#include <Platform/Vulkan/CL_VKRenderer.h>

#include "carnival/Event/ApplicationEvent.h"
#include "carnival/Event/KeyEvent.h"
#include "carnival/Event/MouseEvent.h"

namespace Carnival 
{
	static constexpr Carnival::RenderAPI r_API = RenderAPI::VULK; // TODO : Runtime

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

// ================================================ CONSTRUCTOR =================================================================
	WindowImpl::WindowImpl(const WindowProperties& props) 
		: m_Title{ props.Title }, m_Width{ props.Width }, m_Height{ props.Height }, m_Renderer{ nullptr }, m_VSync{ true }
	{
		CL_CORE_INFO("Creating window {0} ({1} x {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)	Init();

		if constexpr (r_API == RenderAPI::OGL)	
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		if constexpr (r_API == RenderAPI::VULK)	
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);


		m_Window = glfwCreateWindow(props.Width, props.Height, props.Title.c_str(), nullptr, nullptr); // Implicit conversion to int
		if (m_Window) {
			s_WindowCount++;

			if constexpr (r_API == RenderAPI::OGL)
				m_Renderer = std::make_unique<OpenGLRenderer>(m_Window, m_VSync);

			if constexpr (r_API == RenderAPI::VULK)
				m_Renderer = std::make_unique<CL_VKRenderer>(m_Window, m_VSync);
			
			glfwSetWindowUserPointer(m_Window, this);
			SetCallbacks();
		}
		else CL_CORE_ERROR("GLFW Window Creation Failed!");
	}

//================================================ DESTRUCTOR ================================================================

	WindowImpl::~WindowImpl()
	{
		CL_CORE_INFO("Destroying Window {0}", m_Title);
		//delete m_Renderer; // TODO : Unique Pointer?
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
		m_Renderer->drawFrame();
	}

	void WindowImpl::SetVSync(bool enabled)
	{
		m_Renderer->setSwapInterval(enabled);
		m_VSync = enabled;
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
			WindowImpl& data = *(WindowImpl*)glfwGetWindowUserPointer(window);
			data.m_Width = width;
			data.m_Height = height;

			WindowResizeEvent ev(width, height);
			data.m_EventCallback(ev);
		});

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowImpl& data = *(WindowImpl*)glfwGetWindowUserPointer(window);
				data.m_Width = width;
				data.m_Height = height;
				data.m_Renderer->framebufferResizeCallback();
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowImpl& data = *(WindowImpl*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.m_EventCallback(event);
		});

		// Convert Keycodes to engine specific rather than platform specific
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowImpl& data = *(WindowImpl*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.m_EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.m_EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.m_EventCallback(event);
					break;
				}
			}
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowImpl& data = *(WindowImpl*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.m_EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.m_EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowImpl& data = *(WindowImpl*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent event(xOffset, yOffset);
			data.m_EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowImpl& data = *(WindowImpl*)glfwGetWindowUserPointer(window);
			MouseMovedEvent event(xPos, yPos);
			data.m_EventCallback(event);
		});
	}
}