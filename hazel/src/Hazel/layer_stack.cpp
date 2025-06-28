#include "hzpch.h"
#include "layer_stack.h"

namespace Hazel
{

LayerStack::LayerStack()
{
    m_layer_insert = m_layers.begin();
}

LayerStack::~LayerStack()
{
    for (auto layer : m_layers) {
        delete layer;
    }
}

void LayerStack::pushLayer(Layer* layer)
{
    m_layer_insert = m_layers.emplace(m_layer_insert, layer);
}

void LayerStack::pushOverlay(Layer* overly)
{
    m_layers.emplace_back(overly);
}

void LayerStack::popLayer(Layer* layer)
{
    auto it = std::find(m_layers.begin(), m_layers.end(), layer);
    if (it != m_layers.end()) {
        m_layers.erase(it);
        // TODO: m_layer_insertw为何是--?
        m_layer_insert--;
    }
}

void LayerStack::popOverlay(Layer* overly)
{
    auto it = std::find(m_layers.begin(), m_layers.end(), overly);
    if (it != m_layers.end()) {
        m_layers.erase(it);
    }
}

}  // namespace Hazel