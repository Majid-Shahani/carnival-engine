workspace "Carnival"
	architecture "x64"
	
	configurations{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.system}%{cfg.architecture}-%{cfg.buildcfg}"	

project "Core"
	location "Core"
	kind "SharedLib"
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
		"vendor/spdlog/include"
	}
	
	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
		systemversion "latest"
		
		defines
		{
			"CL_PLATFORM_WINDOWS",
			"CL_BUILD_DLL"
		}
		
	filter "configurations:Debug"
		defines "CL_DEBUG"
		symbols "Full"
		optimize "Off"
			
	filter "configurations:Release"
		defines "CL_RELEASE"
		optimize "On"
		symbols "Off"
			
	filter "configurations:Dist"
		defines "CL_DIST"
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
	
	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
		systemversion "latest"
		
		defines
		{
			"CL_PLATFORM_WINDOWS"
		}
		
		links
		{
			"Core"
		}
		
		postbuildcommands
		{
			("{COPYFILE} ../bin/" .. outputdir .. "/Core/Core.dll %{cfg.buildtarget.directory}")
		}
		
	filter "configurations:Debug"
		defines "CL_DEBUG"
		symbols "Full"
		optimize "Off"
			
	filter "configurations:Release"
		defines "CL_RELEASE"
		optimize "On"
		symbols "Off"
			
	filter "configurations:Dist"
		defines "CL_DIST"
		optimize "Full"
		symbols "Off"
			