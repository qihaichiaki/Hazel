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
    HAZEL_API static void init()
    {
        s_renderer_api->init();
    }

    /// @brief 设置清屏颜色
    HAZEL_API static void setClearColor(const glm::vec4& clear_color)
    {
        s_renderer_api->setClearColor(clear_color);
    }

    /// @brief 颜色缓冲区和深度缓冲区使用清屏颜色清屏
    HAZEL_API static void clear()
    {
        s_renderer_api->clear();
    }

    /// @brief 根据索引绘制
    /// @param vertex_array 顶点数组数据
    HAZEL_API static void drawIndexed(const Ref<VertexArray>& vertex_array, uint32_t index_count)
    {
        s_renderer_api->drawIndexed(vertex_array, index_count);
    }

    /// @brief 设置渲染器的渲染视图
    HAZEL_API static void setViewport(uint32_t width, uint32_t height)
    {
        s_renderer_api->setViewport(width, height);
    }

private:
    static Scope<RendererAPI> s_renderer_api;
};
}  // namespace Hazel