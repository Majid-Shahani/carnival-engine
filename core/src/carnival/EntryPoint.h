#pragma once

import Application;


#ifdef CL_PLATFORM_WINDOWS

int main(int argc, char** argv) {
	auto app = Carnival::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif