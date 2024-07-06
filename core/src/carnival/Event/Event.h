#pragma once

#include "../macros.h" // change to <Carnival/macros.h> after project re-set

#include <cstdint>
#include <string>
#include <functional>
#include <iostream>



namespace Carnival
{

	// Make a Buffered Handler

	enum class EventType : uint8_t
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFoucs, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory : uint8_t
	{
		None = 0,
		Application = BIT(0),
		Input = BIT(1),
		Keyboard = BIT(2),
		Mouse = BIT(3),
		MouseButton = BIT(4)
	};

	class CL_API Event
	{
		friend class EventDispatcher;
	public:
#ifdef CL_DEBUG
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
#endif
		virtual EventType	GetEventType() const = 0;
		virtual int			GetCategoryFlags() const = 0;

		inline bool IsInCategory(EventCategory category) const
		{
			return GetCategoryFlags() & category; // AND Operation
		}

	protected:
		bool m_Handled = false;
	};

/*----------------------------------MACROS-------------------------------------------//
* Done this way with macros and enums to improve performance, as opposed to using RTTI
*/
#ifdef CL_DEBUG
	inline std::ostream& operator<<(std::ostream& os, const Event& e) 
	{
		return os << e.ToString();
	}
#define EVENT_CLASS_TYPE(type)	static EventType GetStaticType(){ return EventType::##type;}\
								virtual EventType GetEventType() const override { return GetStaticType();}\
								virtual const char* GetName() const override { return #type;}
#else
#define EVENT_CLASS_TYPE(type)	static EventType GetStaticType(){ return EventType::##type;}\
								virtual EventType GetEventType() const override { return GetStaticType();}
#endif

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }
//-----------------------------------END OF MACROS------------------------------------//
	/*
	template<typename T>
	concept IsEvent = requires {
		//std::convertible_to<T ,Event>;
	}; */

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}
		// This shit doesn't even work, expect redesign or skill up cause I've no clue how to make it work or why it's here
		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	//////////// TO BE CHECKED AFTER SUB CLASSES ARE MADE, MIGHT NOT BE ABLE TO STATIC CAST


}