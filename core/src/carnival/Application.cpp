#include "clpch.h"
#include "Application.h"

#include "Event/ApplicationEvent.h"
#include "Log.h"

namespace Carnival {

	Application::Application() 
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Running = true;
	}

	Application::~Application() {}


	void Application::Run() {
		while (m_Running) 
		{
			m_Window->OnUpdate();
		}
	}
}