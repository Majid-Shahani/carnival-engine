#include "clpch.h"
#include "Application.h"

#include "Event/ApplicationEvent.h"
#include "Log.h"

namespace Carnival {

	Application::Application() 
	{
		Init();
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Running = true;
	}

	Application::~Application() 
	{
		Shutdown();
	}

	void Application::Run() 
	{
		while (m_Running) 
		{
			m_Window->OnUpdate();
		}
	}

	void Application::Init()
	{
		Carnival::Log::Init();
		CL_CORE_CRITICAL("Initialized Log!");
	}

	void Application::Shutdown()
	{
		Window::Shutdown();
	}
}