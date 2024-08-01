#include "clpch.h"
#include "Application.h"
#include <glad/glad.h>

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

		// temporary
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f ,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

		unsigned int indices[3] = { 0,1,2 };
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// temporary

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
			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
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