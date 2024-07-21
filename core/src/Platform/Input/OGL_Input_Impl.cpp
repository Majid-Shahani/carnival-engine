#include <clpch.h>
#ifdef CL_OGL
#include "InputImpl.h"
#include <carnival/Window.h>
#include <carnival/Application.h>

#include <GLFW/glfw3.h>

namespace Carnival {

	Carnival::Input* Input::s_Instance = new InputImpl();

	bool InputImpl::IsKeyPressedImpl(uint16_t keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return (state == GLFW_PRESS || state == GLFW_REPEAT);
	}

	bool InputImpl::IsMouseButtonPressedImpl(uint8_t button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<double, double> InputImpl::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		return { xPos, yPos };
	}
	double InputImpl::GetMouseXImpl()
	{
		auto [x, y] = GetMousePositionImpl();
		return x;
	}

	double InputImpl::GetMouseYImpl()
	{
		auto [x, y] = GetMousePositionImpl();
		return y;
	}
}

#endif