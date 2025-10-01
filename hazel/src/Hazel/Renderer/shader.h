#pragma once

/**
 * shader抽象类
 */

#include <glm/glm.hpp>

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

    /// @brief 返回shader标识
    virtual const std::string& getName() const = 0;

    /// @brief shader设置4x4矩阵统一变量
    /// @param name 变量名
    /// @param value Mat4值
    virtual void setMat4(const std::string& name, const glm::mat4& value) = 0;
    /// @brief shader设置4维坐标统一变量
    /// @param name 变量名
    /// @param value vec4值
    virtual void setFloat4(const std::string& name, const glm::vec4& value) = 0;
    /// @brief shader设置整数统一变量
    /// @param name 统一变量名
    /// @param value int值
    /// @note opengl中,可为shader中的纹理采样器绑定纹理槽
    virtual void setInt(const std::string& name, int value) = 0;
    /// @brief shader设置浮点数统一变量
    /// @param name 统一变量名
    /// @param value float值
    virtual void setFloat(const std::string& name, float value) = 0;

public:
    /// @brief 将顶点/片段shader编译, 整合到shader程序中, 最后进行链接
    /// @param name shader的自定义标识
    /// @param vertex_src 顶点着色器, 获取顶点位置数据, 最后输出顶点位置给gpu渲染
    /// @param fragment_src 片段着色器, 最后输出当前像素颜色给gpu渲染颜色
    static Ref<Shader> create(const std::string& name,
                              const std::string& vertex_src,
                              const std::string& fragment_src);

    /// @brief 通过文件读取shader相关程序
    /// @param file_path shader文件路径
    static Ref<Shader> create(const std::string& file_path);
};

// ShaderLib
class ShaderLibrary
{
public:
    /// @brief 将传入的shader以自定义名字传入
    /// @param name 自定义名
    /// @param shader shader对象
    HAZEL_API void add(const std::string& name, const Ref<Shader>& shader);

    /// @brief 载入shader, 以shader的name为key
    /// @param shader shader对象
    HAZEL_API void add(const Ref<Shader>& shader);

    /// @brief 以路径和自定义名字加载shader
    /// @param name 自定义名
    /// @param file_path shader资源路径
    HAZEL_API Ref<Shader> load(const std::string& name, const std::string& file_path);

    /// @brief 以路径加载shader, 资源文件名作为shader的key
    /// @param file_path shader资源路径
    HAZEL_API Ref<Shader> load(const std::string& file_path);

    /// @brief 获取shader
    HAZEL_API Ref<Shader> get(const std::string& name) const;

    /// @brief 判断此name是否存在于shader库中
    HAZEL_API bool isExists(const std::string& name) const;

private:
    std::unordered_map<std::string, Ref<Shader>> m_shaders;
};

}  // namespace Hazel