#pragma once

#include "Event.h"

namespace Carnival {
	
	class KeyEvent : public Event
	{
	public:
		inline uint16_t GetKeyCode() const { return m_KeyCode; }
	
		EVENT_CLASS_CATEGORY(KeyboardEC | InputEC)

	protected:
		KeyEvent(uint16_t keycode) : m_KeyCode(keycode){}
		uint16_t m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(uint16_t keycode, uint16_t repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount){}

		inline uint16_t GetRepeatCount() const { return m_RepeatCount; }
		EVENT_CLASS_TYPE(KeyPressed)

		#ifdef CL_DEBUG
				std::string ToString() const override
				{
					std::stringstream ss;
					ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
					return ss.str();
				}
		#endif

	private:
		uint16_t m_RepeatCount;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(uint16_t keycode) : KeyEvent(keycode) {}
		EVENT_CLASS_TYPE(KeyReleased)

		#ifdef CL_DEBUG
				std::string ToString() const override
				{
					std::stringstream ss;
					ss << "KeyReleasedEvent: " << m_KeyCode;
					return ss.str();
				}
		#endif
	};
}