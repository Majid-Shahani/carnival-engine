#pragma once

#include "macros.h"
#include "Window.h"

namespace Carnival {

	class CL_API Application {
	public:
		Application();
		virtual ~Application();
		void Run();

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running;
	};

	Carnival::Application* CreateApplication(); // TO BE DEFINED IN THE CLIENT // THIS IS NOT THE ENUM TYPE >:(
}