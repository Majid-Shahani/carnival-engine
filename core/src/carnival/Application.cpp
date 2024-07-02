#include "Application.h"

#include "Event/ApplicationEvent.h"
#include "Log.h"
#include <iostream>

namespace Carnival {
	Application::Application() {

	}

	Application::~Application() {

	}

	void Application::Run() {

		WindowResizeEvent e(1280, 720);
		std::cout << e;

		while (true);
	}

}