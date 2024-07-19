#pragma once

#include "carnival/Window.h"


#ifdef CL_VK
	#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

namespace Carnival {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties& props);
		virtual ~WindowsWindow();

		WindowsWindow(const WindowsWindow&) = delete;
		WindowsWindow& operator=(const WindowsWindow&) = delete;

		void OnUpdate() override;

		inline std::string GetTitle() const override { return m_Data.Title;}
		inline uint16_t GetWidth() const override { return m_Data.Width; }
		inline uint16_t GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void* GetNativeWindow() const { return m_Window; }
	private:
		// Member Functions 

		virtual void InitGLFW();
		virtual void SetCallbacks();
		// virtual void Shutdown();

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
#ifdef CL_VK
	public:
		virtual void InitVK();
#endif
	};
}

