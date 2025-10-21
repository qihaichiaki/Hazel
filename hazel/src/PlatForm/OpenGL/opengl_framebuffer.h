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

    void resize(uint32_t width, uint32_t height) override;

    uint32_t getColorAttachmentRendererID(uint32_t index = 0) const override
    {
        return m_color_attachments[index];
    }

    int readPixel(uint32_t attachment_index, int x, int y) const override;

    const FramebufferSpecification& getSpecification() const override
    {
        return m_specification;
    }

private:
    uint32_t m_rendrer_id = 0;
    FramebufferSpecification m_specification;

    std::vector<FramebufferTextureSpecification> m_color_attachment_specs;
    FramebufferTextureSpecification m_depth_attachment_spec;

    std::vector<uint32_t> m_color_attachments;
    uint32_t m_depth_attachment = 0;
};
}  // namespace Hazel