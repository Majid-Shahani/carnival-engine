#include "clpch.h"
#include "Application.h"

#include "Event/ApplicationEvent.h"
#include "Log.h"

namespace Carnival {
	Application::Application() {

	}

	Application::~Application() {

	}


	void Application::Run() {

		WindowResizeEvent ev(1280, 720);
		CL_INFO(ev.ToString()); // ostr.h requires custom sink I guess, removed for now
		if (ev.IsInCategory(EventCategory::Input)) CL_CRITICAL(ev.ToString());


		while (true);
	}
}