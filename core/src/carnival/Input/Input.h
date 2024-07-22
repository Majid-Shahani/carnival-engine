#pragma once

#include <carnival/macros.h>
#include <cstdint>

namespace Carnival {

	class Input
	{
	public:
		static bool IsKeyPressed(uint16_t keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

		static bool IsMouseButtonPressed(uint8_t button) { return s_Instance->IsMouseButtonPressedImpl(button); }

		static std::pair<double, double> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
		static double GetMouseX(){ return s_Instance->GetMouseXImpl();}
		static double GetMouseY() { return s_Instance->GetMouseYImpl(); }
	protected:
		virtual bool IsKeyPressedImpl(uint16_t ketcode) = 0;

		virtual bool IsMouseButtonPressedImpl(uint8_t button) = 0;

		virtual std::pair<double, double> GetMousePositionImpl() = 0;
		virtual double GetMouseXImpl() = 0;
		virtual double GetMouseYImpl() = 0;
	private:
		static Input* s_Instance;
	};
}