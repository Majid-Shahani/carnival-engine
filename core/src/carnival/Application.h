#pragma once

#include "macros.h"

#include "Window.h"
#include <carnival/Event/ApplicationEvent.h>
#include "Layer/LayerStack.h"
#include "ImGui/ImGuiLayer.h"


namespace Carnival {

	class CL_API Application { // Class is meant to be singleton
	public:
		Application();
		virtual ~Application();
		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		// Functions
		bool OnWindowClose(WindowCloseEvent& e);

		// Variables
		std::unique_ptr<Window> m_Window;
		bool m_Running;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;

		static Application* s_Instance;
	};

	Carnival::Application* CreateApplication(); // TO BE DEFINED IN THE CLIENT
}