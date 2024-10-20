![Carnival_Engine](https://github.com/user-attachments/assets/1bbba43b-fbb1-4998-a374-24b88ea983ed)

Carnival Engine is a free and open source engine as a university project.

This engine currently **only supports windows x64** .
# Installation
1. **Download And Install** [The Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) alongside `GLM` and `Shader Toolchain Debug Symbols`.
- You can change dependency libraries locations in `Dependencies.lua` file if not installed with the SDK or cloned recursively.
2. **Use** `git clone https://github.com/Majid-Shahani/carnival-engine --recursive` in your directory of choice.
3. **Copy** the files located in `./vendor/_premake_files/` to `./vendor/`
4. **Run** *Make.bat* or `call vendor/premake/premake5.exe vs2022` from the root directory
