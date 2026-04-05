// src/Core/LayerStack.cpp
#include <algorithm>
#include "core/LayerStack.h"
#include "core/Log.h"

namespace DC
{
	void LayerStack::PushLayer(std::unique_ptr<Layer> layer)
	{
		DC_CORE_INFO("Pushing layer: {}", layer->GetName());
		layer->OnAttach();
		m_Layers.push_back(std::move(layer));
	}

	std::unique_ptr<Layer> LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find_if(m_Layers.begin(), m_Layers.end(),
			[layer](const std::unique_ptr<Layer>& ptr)
			{
				return ptr.get() == layer;
			});

		if (it == m_Layers.end()) return nullptr;

		(*it)->OnDetach();
		auto owned = std::move(*it);
		m_Layers.erase(it);
		return owned;
	}

	void LayerStack::Clear()
	{
		for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
			(*it)->OnDetach();
		m_Layers.clear();
	}

	void LayerStack::UpdateAll(float delta)
	{
		for (auto& layer : m_Layers)
		{
			layer->OnUpdate(delta);
		}
	}

	void LayerStack::RenderImGuiAll()
	{
		for (auto& layer : m_Layers)
		{
			layer->OnImGuiRender();
		}
	}
}
