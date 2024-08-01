@echo off
setlocal enabledelayedexpansion
set "directory=%~dp0"

set "extension=.vert"
for /r "%directory%" %%f in (*%extension%) do (
	"%VULKAN_SDK%\Bin\glslc.exe" "%%f" -o "%%f.spv"
)

set "extension=.frag"
for /r "%directory%" %%f in (*%extension%) do (
	"%VULKAN_SDK%\Bin\glslc.exe" "%%f" -o "%%f.spv"
)