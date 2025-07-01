#include "hzpch.h"
#include "layer_stack.h"

namespace Hazel
{

LayerStack::LayerStack() : m_layer_insert_index{0} {}

LayerStack::~LayerStack()
{
    for (auto layer : m_layers) {
        layer->onDetach();
        delete layer;
    }
}

void LayerStack::pushLayer(Layer* layer)
{
    layer->onAttach();
    m_layers.emplace(m_layers.begin() + m_layer_insert_index, layer);
    m_layer_insert_index++;
}

void LayerStack::pushOverlay(Layer* overly)
{
    overly->onAttach();
    m_layers.emplace_back(overly);
}

void LayerStack::popLayer(Layer* layer)
{
    layer->onDetach();
    auto it = std::find(m_layers.begin(), m_layers.end(), layer);
    if (it != m_layers.end()) {
        m_layers.erase(it);
        m_layer_insert_index--;
    }
}

void LayerStack::popOverlay(Layer* overly)
{
    overly->onDetach();
    auto it = std::find(m_layers.begin(), m_layers.end(), overly);
    if (it != m_layers.end()) {
        m_layers.erase(it);
    }
}

}  // namespace Hazel