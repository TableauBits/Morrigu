//
// Created by mathi on 2021-04-06.
//

#ifndef MORRIGU_LAYERSTACK_H
#define MORRIGU_LAYERSTACK_H


#include "Core/Layer.h"

#include <vector>

namespace MRG
{
    class LayerStack
    {
    public:
        LayerStack() = default;
        LayerStack(const LayerStack&) = delete;
        LayerStack(LayerStack&&) = delete;
        ~LayerStack();

        LayerStack& operator=(const LayerStack&) = delete;
        LayerStack& operator=(LayerStack&&) = delete;

        void pushLayer(Layer* newLayer);
        Layer* popLayer();

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
    };
}  // namespace MRG

#endif  // MORRIGU_LAYERSTACK_H
