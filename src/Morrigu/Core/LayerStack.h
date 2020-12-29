#ifndef MRG_CLASS_LAYERSTACK
#define MRG_CLASS_LAYERSTACK

#include "Core/Layer.h"

#include <vector>

namespace MRG
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void pushLayer(Layer* newLayer);
		void pushOverlay(Layer* newOverlay);
		void removeLayer(Layer* layerToRemove);
		void removeOverlay(Layer* overlayToRemove);

		[[nodiscard]] auto begin() { return m_layers.begin(); };
		[[nodiscard]] auto end() { return m_layers.end(); };
		[[nodiscard]] auto cbegin() const { return m_layers.cbegin(); };
		[[nodiscard]] auto cend() const { return m_layers.cend(); };
		[[nodiscard]] auto rbegin() { return m_layers.rbegin(); };
		[[nodiscard]] auto rend() { return m_layers.rend(); };
		[[nodiscard]] auto crbegin() const { return m_layers.crbegin(); };
		[[nodiscard]] auto crend() const { return m_layers.crend(); };

	private:
		std::vector<Layer*> m_layers;
		std::size_t m_layerInsertionIndex = 0;
	};
}  // namespace MRG

#endif
