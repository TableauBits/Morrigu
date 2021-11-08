//
// Created by Mathis Lamidey on 2021-04-06.
//

#include "LayerStack.h"

namespace MRG
{
	LayerStack::~LayerStack()
	{
		for (const auto& layerPtr : m_layers) {
			layerPtr->onDetach();
			delete layerPtr;
		}
	}

	void LayerStack::pushLayer(Layer* newLayer)
	{
		m_layers.emplace_back(newLayer);
		newLayer->onAttach();
	}

	Layer* LayerStack::popLayer()
	{
		MRG_ENGINE_ASSERT(!m_layers.empty(), "Layer stack was empty!")
		const auto layer = m_layers.back();
		m_layers.pop_back();
		return layer;
	}
}  // namespace MRG
