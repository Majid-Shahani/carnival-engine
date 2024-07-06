#pragma once

#include "Event.h"

#include <sstream>

namespace Carnival {
	
	class CL_API KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }
	
		EVENT_CLASS_CATEGORY(Keyboard | Input)

	protected:
		KeyEvent(int keycode) : m_KeyCode(keycode){}
		int m_KeyCode;
	};

	class CL_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, uint32_t repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount){}

		inline int GetRepeatCount() const { return m_RepeatCount; }
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
		uint32_t m_RepeatCount;
	};

	class CL_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}
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