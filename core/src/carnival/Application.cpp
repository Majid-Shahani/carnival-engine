#include "clpch.h"
#include "Application.h"

namespace Carnival {

	Application* Application::s_Instance = nullptr;

	Application::Application() 
	{
		CL_CORE_ASSERT(!s_Instance, "Application already exists.");
		s_Instance = this;

		Carnival::Log::Init();
		CL_CORE_INFO("Initialized Log!");

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		//m_ImGuiLayer = new ImGuiLayer();
		//PushOverlay(m_ImGuiLayer);


		m_Running = true;
	}

	Application::~Application() 
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Run() 
	{
		while (m_Running) 
		{
			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate();
			}

			/*
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
			{
				layer->OnRender();
			}
			m_ImGuiLayer->End();
			*/

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			if (e.m_Handled)
				break;
			(*it)->OnEvent(e);
		}
		//CL_CORE_TRACE("Event {0} : {1}", e.ToString(), (e.m_Handled? "Handled" : "Not Handled"));
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}