#include "clpch.h"
#include "Application.h"
#include "Platform/Vulkan/CL_VKRenderer.h"

namespace Carnival {

	Application* Application::s_Instance = nullptr;
	static RenderAPI s_API = RenderAPI::VULK;

	Application::Application() 
	{
		CL_CORE_ASSERT(!s_Instance, "Application already exists.");
		s_Instance = this;

		Carnival::Log::Init();
		CL_CORE_INFO("Initialized Log!");

		WindowProperties carnival{
			.Title = "Carnival Engine",
			.Width = 1280,
			.Height = 720,
			.API = s_API
		};
		m_Window = Window::Create(carnival);
		m_Window->setEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		
		m_Renderer = Renderer::Create(s_API, m_Window, true);

		//m_ImGuiLayer = new ImGuiLayer();
		//PushOverlay(m_ImGuiLayer);

		m_Running = true;
	}

	Application::~Application() 
	{
		delete m_Window;
	}

	void Application::Run() 
	{
		while (m_Running) 
		{
			m_Renderer->clear();
			m_Window->onUpdate();
			m_Renderer->drawFrame();

			/*
			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate();
				layer->OnRender();
			}
			m_ImGuiLayer->Begin();
			m_ImGuiLayer->OnRender();
			m_ImGuiLayer->End();
			*/

			m_Renderer->swapBuffers();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<FrameBufferResizeEvent>(std::bind(&Application::OnFramebufferResize, this, std::placeholders::_1));
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));


		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			if (e.m_Handled)
				break;
			(*it)->OnEvent(e);
		}
		CL_CORE_TRACE("Event {0} : {1}", e.ToString(), (e.m_Handled? "Handled" : "Not Handled"));
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

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
	bool Application::OnFramebufferResize(FrameBufferResizeEvent& e)
	{
		m_Renderer->framebufferResizeCallback();
		return true;
	}
}