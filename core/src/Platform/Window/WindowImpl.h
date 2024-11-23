#pragma once

#include "carnival/Window.h"

namespace Carnival {

	class WindowImpl : public Window
	{
	public:
		WindowImpl(const WindowProperties& props);
		virtual ~WindowImpl();

		// TODO: Can have them create a new window / renderer instance with the same properties
		WindowImpl(const WindowImpl&) = delete;
		WindowImpl& operator=(const WindowImpl&) = delete;

		void onUpdate() override;

		virtual std::string getTitle() const override { return m_Title;}
		virtual uint16_t getWidth() const override { return m_Width; }
		virtual uint16_t getHeight() const override { return m_Height; }

		// Window attributes
		virtual void setEventCallback(const EventCallbackFn& callback) override { m_EventCallback = callback; }

		virtual void* getNativeWindow() const override { return m_Window; }
	private:
		// Member Functions 
		void init();
		void setCallbacks();
		// Member Variables

		std::string m_Title;
		uint16_t m_Width, m_Height;
		EventCallbackFn m_EventCallback;
		GLFWwindow* m_Window;

		// Static Variables
		inline static uint16_t s_WindowCount{};
	};
}

