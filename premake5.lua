include "Dependencies.lua"

workspace "Carnival"
	architecture "x64"
	startproject "Sandbox"
	
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
