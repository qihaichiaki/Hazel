#pragma once

/**
 * 渲染高级类
 */

#include "renderer_command.h"

namespace Hazel
{

class Renderer
{
public:
    /// @brief 渲染场景的开始
    static void beginScene();

    /// @brief 上传顶点数组相关数据
    /// @note 当前未实现渲染队列, 这里提交相当于实时渲染绘制数据
    static void submit(const std::shared_ptr<VertexArray>& vertex_array);

    /// @brief 场景数据准备完毕
    static void endScene();

    /// @brief 返回当前渲染器的具体使用API类型
    static RendererAPI::API getRendererAPI()
    {
        return RendererAPI::getAPI();
    }
};
}  // namespace Hazel