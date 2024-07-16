project "Core"
	kind "SharedLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "Off"
	
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
		"%{IncludeDir.glm}"
	}
	
	links
	{
		"GLFW",
		"opengl32.lib",
		"%{Library.Vulkan}"
	}
	

	filter "system:windows"
		systemversion "latest"
		defines
		{
			"CL_PLATFORM_WINDOWS",
			"CL_BUILD_DLL"
		}
		
	filter { "options:using-vulkan" }
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
			"CL_VK"
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