project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++latest"
	staticruntime "Off"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"src/**.h",
		"src/**.cpp"
	}
	
	includedirs
	{
		"%{wks.location}/Core/src",
		"%{wks.location}/vendor",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}"
	}
	
		links
	{
		"Core"
	}
	
	filter "system:windows"
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
		optimize "Off"
		symbols "Full"

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