#pragma once

/**
 * shader演示类, 后续需要搬迁
 */

#include <glm/glm.hpp>

namespace Hazel
{

class Shader
{
public:
    /// @brief 将顶点/片段shader编译, 整合到shader程序中, 最后进行链接
    /// @param vertex_src 顶点着色器, 获取顶点位置数据, 最后输出顶点位置给gpu渲染
    /// @param fragment_src 片段着色器, 最后输出当前像素颜色给gpu渲染颜色
    Shader(const std::string& vertex_src, const std::string& fragment_src);
    /// @brief 删除当前shader程序
    ~Shader();

    /// @brief 使用shader程序
    void bind();
    /// @brief 停止使用shader程序
    void unBind();
    /// @brief 上传指定名字的统一变量到shader中去
    /// @param uniform_name 在shader中的统一变量名
    /// @param matrix 待上传的矩阵
    void uploadUniformMat4(const std::string& uniform_name, const glm::mat4& matrix);

private:
    uint32_t m_renderer_id;
};

}  // namespace Hazel