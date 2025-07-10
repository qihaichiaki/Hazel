#pragma once

#include "Hazel/Renderer/shader.h"
#include <glm/glm.hpp>

typedef unsigned int GLenum;
namespace Hazel
{
class HAZEL_API OpenGLShader : public Shader
{
public:
    /// @brief 将顶点/片段shader编译, 整合到shader程序中, 最后进行链接
    /// @param vertex_src 顶点着色器, 获取顶点位置数据, 最后输出顶点位置给gpu渲染
    /// @param fragment_src 片段着色器, 最后输出当前像素颜色给gpu渲染颜色
    OpenGLShader(const std::string& vertex_src, const std::string& fragment_src);
    /// @brief 将带有顶点/片段/...的shader程序进行编译, 整合到shader程序中, 最后进行链接
    /// @param file_path shader文件路径
    /// @note ``#type``为shader文件标识, 具体分类为: ``vertex``(顶点shader)/``fragment`` or
    /// ``pixel``(片段着色器)
    OpenGLShader(const std::string& file_path);
    /// @brief 删除当前shader程序
    ~OpenGLShader();

    /// @brief 使用shader程序
    void bind() const override;
    /// @brief 停止使用shader程序
    void unBind() const override;

    /// @brief 上传指定名字的统一变量到shader中去
    /// @param uniform_name 在shader中的统一变量名
    /// @param matrix 待上传的矩阵
    void uploadUniformMat4(const std::string& uniform_name, const glm::mat4& matrix);
    /// @brief 上传指定名字的统一变量到shader中去
    /// @param uniform_name 在shader中的统一变量名
    /// @param vec4 待上传的四维变量
    void uploadUniformFloat4(const std::string& uniform_name, const glm::vec4& vec4);
    void uploadUniformInt(const std::string& uniform_name, int value);

private:
    std::string readFile(const std::string& file_path);  // 读取文件内容到字符缓冲区中
    std::unordered_map<GLenum, std::string> preProcess(
        const std::string& source);  // 将字符缓冲区中的shader程序按照shader类型进行拆分
    void compile(const std::unordered_map<GLenum, std::string>&
                     shader_sources);  // 对所有shader类型的程序进行编译链接
private:
    uint32_t m_renderer_id;
};

}  // namespace Hazel