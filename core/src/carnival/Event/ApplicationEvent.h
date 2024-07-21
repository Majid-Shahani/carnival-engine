#pragma once
#include "Event.h"

namespace Carnival 
{
	class CL_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height)
			: m_Width(width), m_Height(height) 
		{}
		
		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const{ return m_Height; }

		EVENT_CLASS_CATEGORY(ApplicationEC)
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

	class CL_API WindowCloseEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(ApplicationEC)

		#ifdef CL_DEBUG
			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "WindowCloseEvent: Window Closed!";
				return ss.str();
			}
		#endif
	};

	class CL_API AppTickEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(ApplicationEC)
		#ifdef CL_DEBUG
				std::string ToString() const override
				{
					std::stringstream ss;
					ss << "AppTickEvent!";
					return ss.str();
				}
		#endif
	};

	class CL_API AppUpdateEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(ApplicationEC)
		#ifdef CL_DEBUG
				std::string ToString() const override
				{
					std::stringstream ss;
					ss << "AppUpdateEvent!";
					return ss.str();
				}
		#endif
	};

	class CL_API AppRenderEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(ApplicationEC)
		#ifdef CL_DEBUG
				std::string ToString() const override
				{
					std::stringstream ss;
					ss << "AppRenderEvent!";
					return ss.str();
				}
		#endif
	};
}