workspace "Carnival"
	architecture "x64"
	
	configurations{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.system}%{cfg.architecture}-%{cfg.buildcfg}"	

IncludeDir = {}
IncludeDir["GLFW"] = "./vendor/GLFW/include"
IncludeDir["spdlog"] = "./vendor/spdlog/include"

include "vendor/GLFW"

project "Core"
	location "Core"
	kind "SharedLib"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "clpch.h"
	pchsource "Core/src/clpch.cpp"
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}"
	}
	
	links
	{
		"GLFW",
		"opengl32.lib"
	}
	
	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "Off"
		systemversion "latest"
		
		defines
		{
			"CL_PLATFORM_WINDOWS",
			"CL_BUILD_DLL"
		}
		
	filter "configurations:Debug"
		defines 
		{
			"CL_DEBUG",
			"CL_ENABLE_ASSERTS"
		}
		symbols "Full"
		runtime "Debug"
		optimize "Off"
			
	filter "configurations:Release"
		defines "CL_RELEASE"
		runtime "Release"
		optimize "On"
		symbols "Off"
			
	filter "configurations:Dist"
		defines "CL_DIST"
		runtime "Release"
		optimize "Full"
		symbols "Off"			
			
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"Core/src",
		"vendor/spdlog/include"
	}
	
		links
	{
		"Core"
	}
	
	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "Off"
		systemversion "latest"
		
		defines
		{
			"CL_PLATFORM_WINDOWS"
		}
		
		postbuildcommands
		{
			("{COPYFILE} ../bin/" .. outputdir .. "/Core/Core.dll %{cfg.buildtarget.directory}")
		}
		
	filter "configurations:Debug"
		defines "CL_DEBUG"
		runtime "Debug"
		symbols "Full"
		optimize "Off"
			
	filter "configurations:Release"
		defines "CL_RELEASE"
		runtime "Release"
		optimize "On"
		symbols "Off"
			
	filter "configurations:Dist"
		defines "CL_DIST"
		runtime "Release"
		optimize "Full"
		symbols "Off"
			