#pragma once

#include "carnival/Window.h"
#include <carnival/Renderer/Renderer.h>
struct GLFWwindow;

namespace Carnival {

	class WindowImpl : public Window
	{
	public:
		WindowImpl(const WindowProperties& props);
		virtual ~WindowImpl();

		// TODO: Can have them create a new window / renderer instance with the same properties
		WindowImpl(const WindowImpl&) = delete;
		WindowImpl& operator=(const WindowImpl&) = delete;

		void OnUpdate() override;

		virtual std::string GetTitle() const override { return m_Title;}
		virtual uint16_t GetWidth() const override { return m_Width; }
		virtual uint16_t GetHeight() const override { return m_Height; }

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) override { m_EventCallback = callback; }
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override { return m_VSync; }

		virtual void* GetNativeWindow() const override { return m_Window; }
	private:
		// Member Functions 
		void Init();
		void SetCallbacks();
		// Member Variables

		std::string m_Title;
		uint16_t m_Width, m_Height;
		bool m_VSync;
		EventCallbackFn m_EventCallback;
		std::unique_ptr<Renderer> m_Renderer;
		GLFWwindow* m_Window;

		// Static Variables
		inline static uint16_t s_WindowCount{};
	};
}

