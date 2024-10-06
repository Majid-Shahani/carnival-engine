project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"imconfig.h",
		"imgui.h",
		"imgui.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_tables.cpp",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
		"imgui_demo.cpp",
	}

	includedirs
	{
		"%{IncludeDir.ImGui}"
	}

	filter "system:windows"
		systemversion "latest"
		includedirs 
		{
			"%{IncludeDir.GLFW}",
			"%{IncludeDir.VulkanSDK}",
			"%{IncludeDir.SDL2}"
		}
		defines { "IMGUI_IMPL_OPENGL_LOADER_GLAD" } -- Have to change this later?
		files
		{
		-- Backend / Implementation Files
			"backends/imgui_impl_glfw.cpp",
			"backends/imgui_impl_glfw.h",
			"backends/imgui_impl_opengl3.cpp",
			"backends/imgui_impl_opengl3.h",
			"backends/imgui_impl_vulkan.cpp",
			"backends/imgui_impl_vulkan.h",
			"backends/imgui_impl_sdl2.h",
			"backends/imgui_impl_sdl2.cpp",
			"backends/imgui_impl_sdlrenderer2.h",
			"backends/imgui_impl_sdlrenderer2.cpp"
		}
		links
		{
			"%{Library.Vulkan}"
		}

	filter "system:linux"
		pic "On"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "Full"
	
	filter "configurations:Dist"
		runtime "Release"
		optimize "Full"