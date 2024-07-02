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

	
// --------------- LOGGING MACROS ------------------------------//
// Core Logger
#define CL_CORE_TRACE(...)		::Carnival::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CL_CORE_INFO(...)		::Carnival::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CL_CORE_WARN(...)		::Carnival::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CL_CORE_ERROR(...)		::Carnival::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CL_CORE_CRITICAL(...)	::Carnival::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Logger
#define CL_TRACE(...)		    ::Carnival::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CL_INFO(...)			::Carnival::Log::GetClientLogger()->info(__VA_ARGS__)
#define CL_WARN(...)			::Carnival::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CL_ERROR(...)			::Carnival::Log::GetClientLogger()->error(__VA_ARGS__)
#define CL_CRITICAL(...)		::Carnival::Log::GetClientLogger()->critical(__VA_ARGS__)
// -------------- END OF LOGGING MACROS ------------------------//


// -------------- EVENT MACROS -------------------//
#define BIT(x) (1 << x)
// there are more macros defined in Event.h for ease of writing subclasses //
