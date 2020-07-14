#ifndef MRG_CLASS_LAYERSTACK
#define MRG_CLASS_LAYERSTACK

#include "Layer.h"

#include <vector>

namespace MRG
{
	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void pushLayer(Layer* newLayer);
		void pushOverlay(Layer* newOverlay);
		void removeLayer(Layer* layerToRemove);
		void removeOverlay(Layer* overlayToRemove);

		std::vector<Layer*>::iterator begin() { return m_layers.begin(); };
		std::vector<Layer*>::iterator end() { return m_layers.end(); };
		std::vector<Layer*>::const_iterator cbegin() const { return m_layers.cbegin(); };
		std::vector<Layer*>::const_iterator cend() const { return m_layers.cend(); };

	private:
		std::vector<Layer*> m_layers;
		std::vector<Layer*>::iterator m_layerInsertor;
	};
}  // namespace MRG

#endif