include "Dependencies.lua"

workspace "Carnival"
	architecture "x64"
	
	configurations{
		"Debug",
		"Release",
		"Dist"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.system}%{cfg.architecture}-%{cfg.buildcfg}"

newoption
{
	trigger = "using-vulkan",
	description = "Build using vulkan renderer"
}
group "Core"
	include "Core"
group ""

group "Misc"
	include "Sandbox"
group ""

group "Dependencies"
	include "vendor/GLFW"
	include "vendor/glad"
	include "vendor/ImGui"
group ""
