#include "LayerStack.h"

namespace MRG
{
	LayerStack::LayerStack() { m_layerInsertionIndex = 0; }
	LayerStack::~LayerStack()
	{
		for (const auto& layerPtr : m_layers) delete layerPtr;
	}

	void LayerStack::pushLayer(Layer* newLayer)
	{
		m_layers.emplace(m_layers.begin() + m_layerInsertionIndex, newLayer);
		++m_layerInsertionIndex;
	}
	void LayerStack::pushOverlay(Layer* newOverlay) { m_layers.emplace_back(newOverlay); }

	void LayerStack::removeLayer(Layer* layerToRemove)
	{
		auto max =
		  (m_layers.begin() + m_layerInsertionIndex == m_layers.end()) ? m_layers.end() : m_layers.begin() + m_layerInsertionIndex + 1;
		auto it = std::find(m_layers.begin(), max, layerToRemove);
		if (it != max) {
			m_layers.erase(it);
			--m_layerInsertionIndex;
		}
	}

	void LayerStack::removeOverlay(Layer* overlayToRemove)
	{
		auto min =
		  (m_layers.begin() + m_layerInsertionIndex == m_layers.end()) ? m_layers.end() : m_layers.begin() + m_layerInsertionIndex + 1;
		auto it = std::find(min, m_layers.end(), overlayToRemove);
		if (it != m_layers.end())
			m_layers.erase(it);
	}
}  // namespace MRG