#pragma once
#include <clpch.h>

namespace Carnival {
	enum class RenderAPI : uint8_t
	{
		NONE = 0,
		OGL = 1, // Open GL
		VULK = 2 // Vulkan
	};

	class Renderer {
	public:
		virtual ~Renderer() {}

		virtual void Init() = 0;
		virtual void DrawFrame() = 0;
		virtual void SetSwapInterval(bool VSync) = 0;
		virtual void FramebufferResizeCallback() = 0;
	};
}