#pragma once

#include "macros.h"
#include "Event/Event.h"

namespace Carnival {

	class CL_API Layer
	{
	public:
		Layer(const std::string& name = "Layer") : m_Name(name), m_Enabled(false) {}
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_Name; }
	protected:
		std::string m_Name;
		bool m_Enabled;
	};

}