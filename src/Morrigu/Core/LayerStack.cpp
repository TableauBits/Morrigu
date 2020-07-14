#include "LayerStack.h"

namespace MRG
{
	LayerStack::LayerStack() { m_layerInsertor = m_layers.begin(); }
	LayerStack::~LayerStack()
	{
		for (const auto& layerPtr : m_layers) delete layerPtr;
	}

	void LayerStack::pushLayer(Layer* newLayer) { m_layerInsertor = m_layers.emplace(m_layerInsertor, newLayer); }
	void LayerStack::pushOverlay(Layer* newOverlay) { m_layers.emplace_back(newOverlay); }

	void LayerStack::removeLayer(Layer* layerToRemove)
	{
		auto max = (m_layerInsertor == m_layers.end()) ? m_layers.end() : m_layerInsertor + 1;
		auto it = std::find(m_layers.begin(), max, layerToRemove);
		if (it != max) {
			m_layers.erase(it);
			--m_layerInsertor;
		}
	}

	void LayerStack::removeOverlay(Layer* overlayToRemove)
	{
		auto min = (m_layerInsertor == m_layers.end()) ? m_layers.end() : m_layerInsertor + 1;
		auto it = std::find(min, m_layers.end(), overlayToRemove);
		if (it != m_layers.end())
			m_layers.erase(it);
	}
}  // namespace MRG