// src/Core/LayerStack.h
#pragma once
#include "core/Layer.h"
#include <vector>
#include <memory>

namespace DC
{
	class LayerStack
	{
	public:
		LayerStack()								= default;
		~LayerStack()								= default;

		LayerStack(LayerStack&&)					= default;
		LayerStack& operator=(LayerStack&&)			= default;

		LayerStack(const LayerStack&)				= delete;
		LayerStack& operator=(const LayerStack&)	= delete;

		void PushLayer(std::unique_ptr<Layer> layer);
		std::unique_ptr<Layer> PopLayer(Layer* layer);
		void Clear();
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
