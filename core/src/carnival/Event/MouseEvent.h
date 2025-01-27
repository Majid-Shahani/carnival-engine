#pragma once

#include "Event.h"

namespace Carnival {
	
	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(double xPos, double yPos) : m_MouseX(xPos), m_MouseY(yPos)
		{}
		EVENT_CLASS_TYPE(MouseMove)
		EVENT_CLASS_CATEGORY(InputEC | MouseEC)
		
		double GetX() const { return m_MouseX; }
		double GetY() const { return m_MouseY; }
		
		#ifdef CL_DEBUG
			std::string ToString() const 
			{
				std::stringstream ss;
				ss << "MouseMovedEvent:\tX: " << m_MouseX << "\tY: " << m_MouseY;
				return ss.str();
			}
		#endif
	private:
		double m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(double XOffset, double YOffset) : m_Xoffset(XOffset), m_Yoffset(YOffset)
		{}
		EVENT_CLASS_TYPE(MouseScroll)
		EVENT_CLASS_CATEGORY(InputEC | MouseEC)

		double GetXoffset() const { return m_Xoffset; }
		double GetYoffset() const { return m_Yoffset; }

		#ifdef CL_DEBUG
				std::string ToString() const
				{
					std::stringstream ss;
					ss << "MouseScrolledEvent:\tX: " << m_Xoffset << "\tY: " << m_Yoffset;
					return ss.str();
				}
		#endif
	private:
		double m_Xoffset, m_Yoffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		inline uint8_t GetMouseButton() const { return m_Button; }
		EVENT_CLASS_CATEGORY(InputEC | MouseEC | MouseButtonEC)
	protected:
		MouseButtonEvent(uint8_t button) : m_Button(button) {}
		uint8_t m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(uint8_t button) : MouseButtonEvent(button)
		{}
		EVENT_CLASS_TYPE(MouseButtonPress)
		#ifdef CL_DEBUG
				std::string ToString() const
				{
					std::stringstream ss;
					ss << "MousePressedEvent: " << static_cast<int>(m_Button);
					return ss.str();
				}
		#endif
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(uint8_t button) : MouseButtonEvent(button)
		{}
		EVENT_CLASS_TYPE(MouseButtonRelease)
		#ifdef CL_DEBUG
				std::string ToString() const
				{
					std::stringstream ss;
					ss << "MouseReleasedEvent: " << static_cast<int>(m_Button);
					return ss.str();
				}
		#endif
	};
}