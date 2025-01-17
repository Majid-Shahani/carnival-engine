#include <clpch.h>
#include "Renderer.h"
#include <Platform/Vulkan/CL_VKRenderer.h>
#include <Platform/OpenGL/OGLRenderer.h>
namespace Carnival {

	std::unique_ptr<Renderer> Renderer::Create(RenderAPI api, void* pWindow, bool vSync) 
	{
		if (api == RenderAPI::VULK) {
			return std::make_unique<CL_VKRenderer>(static_cast<GLFWwindow*>(((Window*)pWindow)->getNativeWindow()), vSync);
		}
		else if (api == RenderAPI::OGL) {
			return std::make_unique<OpenGLRenderer>(static_cast<GLFWwindow*>(((Window*)pWindow)->getNativeWindow()), vSync);
		}
		else {
			CL_CORE_ERROR("Failed To Create Renderer Instance");
			throw std::runtime_error("Failed to create renderer Instance");
		}
	}
}