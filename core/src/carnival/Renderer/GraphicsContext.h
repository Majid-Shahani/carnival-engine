#pragma once

namespace Carnival {

	class GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		virtual void SetSwapInterval(bool VSync) = 0;
	};
}