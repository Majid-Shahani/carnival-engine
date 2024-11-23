#pragma once
#include "macros.h"
#include "Event/Event.h"
#include <carnival/Renderer/Renderer.h>

namespace Carnival {
	struct WindowProperties
	{
		std::string Title;
		uint16_t Width, Height;
		RenderAPI API;
	};

	class Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}		
		virtual void onUpdate() = 0;

		virtual std::string getTitle() const = 0;
		virtual uint16_t getWidth() const = 0;
		virtual uint16_t getHeight() const = 0;

		virtual void setEventCallback(const EventCallbackFn& callback) = 0;

		virtual void* getNativeWindow() const = 0;

		static Window* Create(const WindowProperties& props);
	};
}