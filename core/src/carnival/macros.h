#pragma once

#ifndef CL_PLATFORM_WINDOWS
	#error Carnival Only Supports Windows! :(
#endif

	
// --------------- LOGGING MACROS ------------------------------//
#ifndef CL_DIST
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
#else
	#define CL_CORE_TRACE(...)
	#define CL_CORE_INFO(...)
	#define CL_CORE_WARN(...)
	#define CL_CORE_ERROR(...)
	#define CL_CORE_CRITICAL(...)
	
	#define CL_TRACE(...)   
	#define CL_INFO(...)
	#define CL_WARN(...)
	#define CL_ERROR(...)
	#define CL_CRITICAL(...)
#endif
// -------------- END OF LOGGING MACROS ------------------------//

#ifdef CL_ENABLE_ASSERTS
	#define CL_ASSERT(x, ...) { if(!(x)) { CL_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define CL_CORE_ASSERT(x, ...) { if(!(x)) { CL_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define CL_ASSERT(x, ...)
	#define CL_CORE_ASSERT(x, ...)
#endif

// -------------- EVENT MACROS -------------------//
#define BIT(x) (1 << x)
// there are more macros defined in Event.h for ease of writing subclasses //
