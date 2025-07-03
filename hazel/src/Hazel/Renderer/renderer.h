#pragma once

/**
 * 渲染器接口定义
 */

namespace Hazel
{

enum class RendererAPI { None = 0, OpenGL = 1 };

class Renderer
{
public:
    /// @brief 返回当前渲染器的具体使用API类型
    static RendererAPI getRendererAPI()
    {
        return s_renderer_api;
    }

private:
    static RendererAPI s_renderer_api;
};
}  // namespace Hazel