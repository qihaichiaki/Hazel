#pragma once

/**
 * renderer 命令的静态封装类
 */

#include "renderer_api.h"

namespace Hazel
{

class RendererCommand
{
public:
    /// @brief 设置清屏颜色
    static void setClearColor(const glm::vec4& clear_color)
    {
        s_renderer_api->setClearColor(clear_color);
    }

    /// @brief 颜色缓冲区和深度缓冲区使用清屏颜色清屏
    static void clear()
    {
        s_renderer_api->clear();
    }

    /// @brief 根据索引绘制
    /// @param vertex_array 顶点数组数据
    static void drawIndexed(const std::shared_ptr<VertexArray>& vertex_array)
    {
        s_renderer_api->drawIndexed(vertex_array);
    }

private:
    static RendererAPI* s_renderer_api;
};
}  // namespace Hazel