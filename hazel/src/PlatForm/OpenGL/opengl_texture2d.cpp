#include "hzpch.h"
#include "opengl_texture2d.h"

#include <stb_image.h>
#include <glad/glad.h>

namespace Hazel
{
OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : m_path(path)
{
    stbi_set_flip_vertically_on_load(1);  // 翻转读取图片缓冲区
    int width, height;
    int channels;  // 通道数
    stbi_uc* data = stbi_load(m_path.c_str(), &width, &height, &channels, 0);
    HZ_CORE_ASSERT(data, "OpenGLTexture2D 加载纹理失败!");
    m_width = width;
    m_height = height;

    GLenum internalformat;  // 存储纹理通道类型
    GLenum format;          // 上传纹理时通道类型
    if (channels == 4)      // rgba
    {
        internalformat = GL_RGBA8;
        format = GL_RGBA;
    } else if (channels == 3) {  // rgb
        internalformat = GL_RGB8;
        format = GL_RGB;
    } else {
        HZ_CORE_ASSERT(false, "OpenGLTexture2D 加载纹理不支持通道数为:{}", channels);
        return;
    }

    // opengl 创建texture对象
    glCreateTextures(GL_TEXTURE_2D, 1, &m_renderer_id);
    // 创建存储纹理的区域
    glTextureStorage2D(m_renderer_id, 1, internalformat, m_width, m_height);
    // 设置纹理参数, 这些未来需要暴露给外面进行设置
    // 缩小纹理过滤器设置
    glTextureParameteri(m_renderer_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // 线性插值
    // 放大纹理过滤器设置
    glTextureParameteri(m_renderer_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // 非线性插值, 防止模糊
    // 上传纹理缓冲区数据
    glTextureSubImage2D(m_renderer_id, 0, 0, 0, m_width, m_height, format, GL_UNSIGNED_BYTE, data);

    // 上传完毕后, data数据可以清理
    stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
    glDeleteTextures(1, &m_renderer_id);
}

void OpenGLTexture2D::bind(uint32_t slot) const
{
    glBindTextureUnit(slot, m_renderer_id);  // 将纹理数据绑定到slot插槽
}

}  // namespace Hazel