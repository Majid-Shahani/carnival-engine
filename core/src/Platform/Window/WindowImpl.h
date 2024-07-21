#pragma once

#include "carnival/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Carnival {

	class WindowImpl : public Window
	{
	public:
		WindowImpl(const WindowProperties& props);
		virtual ~WindowImpl();

		WindowImpl(const WindowImpl&) = delete;
		WindowImpl& operator=(const WindowImpl&) = delete;

		void OnUpdate() override;

		inline std::string GetTitle() const override { return m_Data.Title;}
		inline uint16_t GetWidth() const override { return m_Data.Width; }
		inline uint16_t GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override { return m_Data.VSync; }

		virtual void* GetNativeWindow() const override { return m_Window; } // OGL only?
	private:
		// Member Functions 
		virtual void Init();
		virtual void SetCallbacks();

		// Member Variables
		struct WindowData
		{
			WindowData(std::string title, uint16_t width, uint16_t height, bool Vsync = true) 
				: Title(title), Width(width), Height(height), VSync(Vsync)
			{}

			std::string Title;
			uint16_t Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};
		WindowData m_Data;

		GLFWwindow* m_Window;

		// Static Variables
		inline static uint16_t s_WindowCount{};
	};
}

