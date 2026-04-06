// src/Core/Layer.h
#pragma once
#include <string>

namespace DC
{
	class Layer
	{
	public:
		explicit Layer(const std::string& name = "Layer") : m_DebugName(name){}
		virtual ~Layer() = default;

		// Non-pure virtuals: override only what is needed
		// No forced empty boilerplate on layers that don't use all hooks.
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float) {}
		virtual void OnImGuiRender() {}

		const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
}
