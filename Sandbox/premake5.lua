project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++latest"
	staticruntime "Off"
	
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"src/**.h",
		"src/**.cpp"
	}
	
	includedirs
	{
		"%{wks.location}/Core/src",
		"%{wks.location}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}"
	}
	
		links
	{
		"Core"
	}
	
	filter "system:windows"
		systemversion "latest"
		postbuildcommands
		{
			("{COPYFILE} %{wks.location}/bin/" .. outputdir .. "/Core/Core.dll %{cfg.buildtarget.directory}")
		}

	filter { "options:using-vulkan" }
		includedirs
		{
			"%{IncludeDir.VulkanSDK}"
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