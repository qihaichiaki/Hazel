#pragma once

/**
 * renderer api 物理api
 * 不同平台存在不同的api实现
 */

#include <glm/glm.hpp>
#include "vertex_array.h"

namespace Hazel
{

class RendererAPI
{
public:
    enum class API { None = 0, OpenGL = 1 };

public:
    /// @brief 设置清屏颜色
    virtual void setClearColor(const glm::vec4& clear_color) = 0;

    /// @brief 颜色缓冲区和深度缓冲区使用清屏颜色清屏
    virtual void clear() = 0;

    /// @brief 根据索引绘制
    /// @param vertex_array 顶点数组数据
    virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertex_array) = 0;

public:
    /// @brief 获取当前的后端渲染API
    static API getAPI()
    {
        return s_api;
    }

private:
    static API s_api;
};
}  // namespace Hazel