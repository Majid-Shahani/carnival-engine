project "Core"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"
	
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "clpch.h"
	pchsource "src/clpch.cpp"
	
	files
	{
		"src/**.h",
		"src/**.cpp"
	}
	
	includedirs
	{
		"src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}"
	}
	
	links
	{
		"GLFW",
		"ImGui",
		"opengl32.lib"
	}
	

	filter "system:windows"
		systemversion "latest"
		defines
		{
			"CL_PLATFORM_WINDOWS"
		}
		postbuildcommands
		{	
			--("{MKDIR} ../bin/" .. outputdir .. "/Sandbox/"),
			--("{COPYFILE} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}
-------------------------------- OPTIONS ---------------------------		
	filter { "options:api=vulkan" }
		includedirs
		{
			"%{IncludeDir.VulkanSDK}"
		}
		links
		{
			"%{Library.Vulkan}"
		}
		defines
		{
			"CL_VK",
			"GLFW_INCLUDE_VULKAN"
		}
		
	filter { "options:api=opengl" }
		includedirs
		{
			"%{IncludeDir.glad}"
		}
		links
		{
			"glad"
		}
		defines
		{
			"CL_OGL",
			"GLFW_INCLUDE_NONE"
		}
--------------------------------- CONFIGS -------------------------
	filter "configurations:Debug"
		defines 
		{
			"CL_DEBUG",
			"CL_ENABLE_ASSERTS"
		}
		symbols "Full"
		runtime "Debug"
		optimize "Off"
		links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}
			
	filter "configurations:Release"
		defines "CL_RELEASE"
		runtime "Release"
		optimize "On"
		symbols "Off"
		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}
			
	filter "configurations:Dist"
		defines "CL_DIST"
		runtime "Release"
		optimize "Full"
		symbols "Off"
		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}