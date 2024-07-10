#include "clpch.h"
#include "Application.h"

#include "Log.h"

namespace Carnival {

	Application::Application() 
	{
		Carnival::Log::Init();
		CL_CORE_INFO("Initialized Log!");

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		m_Running = true;

	}

	Application::~Application() 
	{
	}

	void Application::Run() 
	{
		while (m_Running) 
		{
			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
		CL_CORE_TRACE(e.ToString());
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}