#pragma once

#include <Hazel/Renderer/framebuffer.h>

namespace Hazel
{

class OpenGLFramebuffer : public Framebuffer
{
public:
    OpenGLFramebuffer(const FramebufferSpecification& spec);
    ~OpenGLFramebuffer();

    void invalidate();

    void bind() const override;
    void unBind() const override;

    uint32_t getColorAttachmentRendererID() const override
    {
        return m_color_attachment;
    }

private:
    uint32_t m_rendrer_id;
    uint32_t m_color_attachment;
    uint32_t m_depth_attachment;
    FramebufferSpecification m_specification;
};
}  // namespace Hazel