// src/Core/LayerStack.h
#pragma once
#include "Core/Layer.h"
#include <vector>
#include <memory>

namespace DC
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack() = default;

		void PushLayer(std::unique_ptr<Layer> layer);
		std::unique_ptr<Layer> PopLayer(Layer* layer);
		void UpdateAll(float delta);
		void RenderImGuiAll();

		auto begin() { return m_Layers.begin(); }
		auto end() { return m_Layers.end(); }
		auto begin() const { return m_Layers.begin(); }
		auto end() const { return m_Layers.end(); }
		size_t Size() const { return m_Layers.size(); }

	private:
		std::vector<std::unique_ptr<Layer>> m_Layers;
	};
}
