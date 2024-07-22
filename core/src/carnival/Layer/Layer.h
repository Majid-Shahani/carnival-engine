#pragma once

#include <carnival/macros.h>
#include <carnival/Event/Event.h>

namespace Carnival {

	class Layer
	{
	public:
		Layer(const std::string& name = "Layer") : m_Name(name){}
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& event) {}


		virtual void OnRender() {};

		inline const std::string& GetName() const { return m_Name; }
	protected:
		std::string m_Name;
	};

}