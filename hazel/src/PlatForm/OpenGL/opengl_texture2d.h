#pragma once

#include "Hazel/Renderer/texture.h"

namespace Hazel
{
class OpenGLTexture2D : public Texture2D
{
public:
    OpenGLTexture2D(const std::string& path);
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

private:
    std::string m_path;  // 文件路径保存

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_renderer_id;
};
}  // namespace Hazel