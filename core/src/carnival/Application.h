#pragma once

#include "macros.h"

#include "Event/Event.h"
#include "Event/ApplicationEvent.h"
#include "Window.h"

namespace Carnival {

	class CL_API Application { // Class is meant to be singleton for now
	public:
		Application();
		virtual ~Application();
		void Run();

		void OnEvent(Event& e);
	private:
		// Functions
		bool OnWindowClose(WindowCloseEvent& e);
		// Variables
		std::unique_ptr<Window> m_Window;
		bool m_Running;
	};

	Carnival::Application* CreateApplication(); // TO BE DEFINED IN THE CLIENT
}