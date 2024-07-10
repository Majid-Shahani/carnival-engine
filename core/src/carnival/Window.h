#pragma once

#include "macros.h"
#include "Event/Event.h"

namespace Carnival {

	struct WindowProperties
	{
		std::string Title;
		uint16_t Width, Height;

		WindowProperties(const std::string& title = "Carnival Engine",
					uint16_t width = 1280, uint16_t height = 720)
					: Title(title), Width(width), Height(height) {}
	};

	class CL_API Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}
		
		virtual void OnUpdate() = 0;
		
		virtual std::string GetTitle() const = 0;
		virtual uint16_t GetWidth() const = 0;
		virtual uint16_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual bool IsVSync() const = 0;
		virtual void SetVSync(bool enabled) = 0;

		static Window* Create(const WindowProperties& props = WindowProperties());
		static void Shutdown();
	};
}