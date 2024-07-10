#pragma once

#include "macros.h"


#ifdef CL_PLATFORM_WINDOWS

extern Carnival::Application* Carnival::CreateApplication();

int main(int argc, char** argv) {

	auto app = Carnival::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif