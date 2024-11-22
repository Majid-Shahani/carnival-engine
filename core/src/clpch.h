#pragma once

// STANDARD LIBRARY
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// CARNIVAL
#include "carnival/Log.h"
#include "carnival/Files/Files.h"

// VENDOR
#include <glm.hpp>
#include <vulkan/vulkan.h>

#ifdef CL_PLATFORM_WINDOWS
	#include <glad/glad.h>
	#include <GLFW/glfw3.h>
	#include <Windows.h>
#endif

