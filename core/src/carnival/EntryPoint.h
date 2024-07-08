#pragma once

#include "macros.h"


#ifdef CL_PLATFORM_WINDOWS

extern Carnival::Application* Carnival::CreateApplication();

int main(int argc, char** argv) {
	Carnival::Log::Init(); // to be moved to general initialization function later

	CL_CORE_CRITICAL("Initialized Log!");

	auto app = Carnival::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif