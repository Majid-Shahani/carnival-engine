#pragma once

#ifdef CL_PLATFORM_WINDOWS
	#ifdef CL_BUILD_DLL
		#define CL_API __declspec(dllexport)
	#else
		#define CL_API __declspec(dllimport)
	#endif
#else
	#error Carnival Only Supports Windows! :(
#endif