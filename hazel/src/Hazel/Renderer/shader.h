#pragma once

/**
 * shader抽象类
 */

namespace Hazel
{

class HAZEL_API Shader
{
public:
    virtual ~Shader() = default;

    /// @brief 使用shader程序
    virtual void bind() const = 0;
    /// @brief 停止使用shader程序
    virtual void unBind() const = 0;

public:
    /// @brief 将顶点/片段shader编译, 整合到shader程序中, 最后进行链接
    /// @param vertex_src 顶点着色器, 获取顶点位置数据, 最后输出顶点位置给gpu渲染
    /// @param fragment_src 片段着色器, 最后输出当前像素颜色给gpu渲染颜色
    static Shader* create(const std::string& vertex_src, const std::string& fragment_src);

    /// @brief 通过文件读取shader相关程序
    /// @param file_path shader文件路径
    static Shader* create(const std::string& file_path);
};

}  // namespace Hazel