#pragma once

#include "Event.h"

#include <sstream>

namespace Carnival 
{
	class CL_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height)
			: m_Width(width), m_Height(height) 
		{
		}
		
		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const{ return m_Height; }

		EVENT_CLASS_CATEGORY(Application)
		EVENT_CLASS_TYPE(WindowResize)


#ifdef CL_DEBUG
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}
#endif


	private:
		uint32_t m_Width, m_Height;
	};
}