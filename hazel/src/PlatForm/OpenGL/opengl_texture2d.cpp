#include "hzpch.h"
#include "opengl_texture2d.h"

#include <stb_image.h>

namespace Hazel
{
OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height) : m_width(width), m_height(height)
{
    HZ_PROFILE_FUNCTION();

    glCreateTextures(GL_TEXTURE_2D, 1, &m_renderer_id);
    m_internal_format = GL_RGBA8;
    m_data_format = GL_RGBA;

    // 创建纹理存储
    glTextureStorage2D(m_renderer_id, 1, m_internal_format, m_width, m_height);
    // 设置纹理属性
    glTextureParameteri(m_renderer_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // 线性插值
    // 放大纹理过滤器设置
    glTextureParameteri(m_renderer_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // 非线性插值, 防止模糊
    // 如果超出纹理坐标的映射(0, 1), 则进行重复利用
    glTextureParameteri(m_renderer_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_renderer_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : m_path(path)
{
    HZ_PROFILE_FUNCTION();

    stbi_set_flip_vertically_on_load(1);  // 翻转读取图片缓冲区
    int width, height;
    int channels;  // 通道数

    stbi_uc* data = nullptr;
    {
        HZ_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
        data = stbi_load(m_path.c_str(), &width, &height, &channels, 0);
    }
    HZ_CORE_ASSERT(data, "OpenGLTexture2D 加载纹理失败!");
    m_width = width;
    m_height = height;

    if (channels == 4)  // rgba
    {
        m_internal_format = GL_RGBA8;
        m_data_format = GL_RGBA;
    } else if (channels == 3) {  // rgb
        m_internal_format = GL_RGB8;
        m_data_format = GL_RGB;
    } else {
        HZ_CORE_ASSERT(false, "OpenGLTexture2D 加载纹理不支持通道数为:{}", channels);
        return;
    }

    // opengl 创建texture对象
    glCreateTextures(GL_TEXTURE_2D, 1, &m_renderer_id);
    // 创建存储纹理的区域
    glTextureStorage2D(m_renderer_id, 1, m_internal_format, m_width, m_height);
    // 设置纹理参数, 这些未来需要暴露给外面进行设置
    // 缩小纹理过滤器设置
    glTextureParameteri(m_renderer_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // 线性插值
    // 放大纹理过滤器设置
    glTextureParameteri(m_renderer_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // 非线性插值, 防止模糊
    // 如果超出纹理坐标的映射(0, 1), 则进行重复利用
    glTextureParameteri(m_renderer_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_renderer_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 上传纹理缓冲区数据
    glTextureSubImage2D(m_renderer_id, 0, 0, 0, m_width, m_height, m_data_format, GL_UNSIGNED_BYTE,
                        data);

    // 上传完毕后, data数据可以清理
    stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
    HZ_PROFILE_FUNCTION();

    glDeleteTextures(1, &m_renderer_id);
}

void OpenGLTexture2D::bind(uint32_t slot) const
{
    HZ_PROFILE_FUNCTION();

    glBindTextureUnit(slot, m_renderer_id);  // 将纹理数据绑定到slot插槽
}

void OpenGLTexture2D::setData(void* data, uint32_t size)
{
    HZ_PROFILE_FUNCTION();

    HZ_CORE_ASSERT((size == m_width * m_height * (m_data_format == GL_RGBA ? 4 : 3)),
                   "当前纹理设置的数据和宽度高度不匹配");

    glTextureSubImage2D(m_renderer_id, 0, 0, 0, m_width, m_height, m_data_format, GL_UNSIGNED_BYTE,
                        data);
}

bool OpenGLTexture2D::isEqual(const Texture& other)
{
    return ((const OpenGLTexture2D&)other).m_renderer_id == m_renderer_id;
}

}  // namespace Hazel