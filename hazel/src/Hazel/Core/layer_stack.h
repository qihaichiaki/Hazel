#pragma once

/**
 * 层栈结构, 从左往右遍历时渲染更新, 从右往左遍历是事件更新
 */
#include "core.h"
#include "layer.h"

namespace Hazel
{

class LayerStack
{
public:
    HAZEL_API LayerStack();
    HAZEL_API ~LayerStack();

    /// @brief 插入到层级栈顶
    HAZEL_API void pushLayer(Layer* layer);
    /// @brief 插入到层级栈底(最后渲染, 画面优先级高)
    HAZEL_API void pushOverlay(Layer* overly);
    HAZEL_API void popLayer(Layer* layer);
    HAZEL_API void popOverlay(Layer* overly);

    HAZEL_API std::vector<Layer*>::iterator begin()
    {
        return m_layers.begin();
    }

    HAZEL_API std::vector<Layer*>::iterator end()
    {
        return m_layers.end();
    }

private:
    std::vector<Layer*> m_layers;
    unsigned int m_layer_insert_index;
};

}  // namespace Hazel