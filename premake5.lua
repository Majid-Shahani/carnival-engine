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

group "Dependencies"
	include "vendor/GLFW"
group ""

group "Core"
	include "Core"
group ""

group "Misc"
	include "Sandbox"
group ""