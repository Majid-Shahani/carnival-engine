#pragma once

#include "macros.h"

namespace Carnival {
	class CL_API Application {
	public:
		Application();
		virtual ~Application();
		void Run();
	};

	Carnival::Application* CreateApplication(); // TO BE DEFINED IN THE CLIENT
}