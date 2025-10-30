#pragma once

#include "Hazel/Renderer/texture.h"
#include <glad/glad.h>

namespace Hazel
{
class OpenGLTexture2D : public Texture2D
{
public:
    OpenGLTexture2D(const std::string& path);
    OpenGLTexture2D(uint32_t width, uint32_t height);
    ~OpenGLTexture2D();

    uint32_t getWidth() const override
    {
        return m_width;
    }
    uint32_t getHeight() const override
    {
        return m_height;
    }

    void bind(uint32_t slot = 0) const override;

    void setData(void* data, uint32_t size) override;

    bool isEqual(const Texture& other) override;

    uint32_t getRendererId() override
    {
        return m_renderer_id;
    }

private:
    std::string m_path;  // 文件路径保存

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_renderer_id;

    GLenum m_internal_format;  // 纹理存储类型
    GLenum m_data_format;      // 纹理上传类型
};
}  // namespace Hazel