#pragma once

#ifdef CL_PLATFORM_WINDOWS

int main(int argc, char** argv) {
	Carnival::Log::Init(); // to be moved to general initialization function later

	Carnival::Log::GetCoreLogger()->warn("Initialized Log!");
	Carnival::Log::GetClientLogger()->info("WILL THIS FINALLY WORK!");

	auto app = Carnival::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif