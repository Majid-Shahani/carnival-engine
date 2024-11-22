#pragma once
#include "macros.h"
#include "Event/Event.h"
#include <carnival/Renderer/Renderer.h>

namespace Carnival {
	struct WindowProperties
	{
		std::string Title;
		uint16_t Width, Height;

		WindowProperties(const std::string& title = "Carnival Engine",
			uint16_t width = 1280, uint16_t height = 720)
			: Title{ title }, Width{ width }, Height{ height }
		{}
	};

	class Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		// temporary
		virtual void clear() = 0;
		virtual void onUpdate() = 0;
		virtual void swapFrame() = 0;

		virtual std::string getTitle() const = 0;
		virtual uint16_t getWidth() const = 0;
		virtual uint16_t getHeight() const = 0;

		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual bool isVSync() const = 0;
		virtual void setVSync(bool enabled) = 0;

		virtual void* getNativeWindow() const = 0;

		static Window* Create(const WindowProperties& props = WindowProperties());
	};
}