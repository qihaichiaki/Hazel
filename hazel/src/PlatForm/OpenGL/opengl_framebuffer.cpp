#include <hzpch.h>

#include "opengl_framebuffer.h"

#include <glad/glad.h>

namespace Hazel
{

static constexpr uint32_t s_max_framebuffer_size = 8192;

static GLenum textureTarget(bool is_multisampled)
{
    return is_multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

static void createTextures(bool is_multisampled, uint32_t* out_id, uint32_t count)
{
    glCreateTextures(textureTarget(is_multisampled), count, out_id);
}

static void bindTexture(bool is_multisampled, uint32_t id)
{
    glBindTexture(textureTarget(is_multisampled), id);
}

static bool isDepthFormat(FramebufferTextureSpecification attachment)
{
    return attachment.TextureFormat == FramebufferTextureFormat::DEPTH24_STENCIL8;
}

static void attachColorTexture(uint32_t id,
                               uint32_t samples,
                               GLenum internal_format,
                               GLenum format,
                               uint32_t width,
                               uint32_t height,
                               uint32_t index)
{
    bool is_multisampled = samples > 1;
    if (is_multisampled) {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, width, height,
                                GL_FALSE);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE,
                     nullptr);

        // filter and wrap
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // 上传到帧缓冲区
    // 贴图附件
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
                           textureTarget(is_multisampled), id, 0);
}

static void attachDepthTexture(uint32_t id,
                               uint32_t samples,
                               GLenum format,
                               GLenum attachment_type,
                               uint32_t width,
                               uint32_t height)
{
    bool is_multisampled = samples > 1;
    if (is_multisampled) {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height,
                                GL_FALSE);
    } else {
        glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
    }

    // 上传到帧缓冲区
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, textureTarget(is_multisampled), id, 0);
}

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec) : m_specification(spec)
{
    // 分类
    for (auto attachment : spec.getAttachmentSpecification().getAttachments()) {
        if (!isDepthFormat(attachment)) {
            m_color_attachment_specs.emplace_back(attachment);
        } else {
            m_depth_attachment_spec = attachment;
        }
    }

    invalidate();
}
OpenGLFramebuffer::~OpenGLFramebuffer()
{
    glDeleteFramebuffers(1, &m_rendrer_id);
    glDeleteTextures((uint32_t)m_color_attachments.size(), m_color_attachments.data());
    glDeleteTextures(1, &m_depth_attachment);
}

void OpenGLFramebuffer::invalidate()
{
    if (m_rendrer_id != 0) {
        glDeleteFramebuffers(1, &m_rendrer_id);
        glDeleteTextures((uint32_t)m_color_attachments.size(), m_color_attachments.data());
        glDeleteTextures(1, &m_depth_attachment);

        m_color_attachments.clear();
        m_depth_attachment = 0;
    }

    // 创建帧缓冲区
    glCreateFramebuffers(1, &m_rendrer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_rendrer_id);

    bool is_multisampled = m_specification.getSamples() > 1;

    // 创建颜色纹理
    if (m_color_attachment_specs.size() > 0) {
        m_color_attachments.resize(m_color_attachment_specs.size());
        createTextures(is_multisampled, m_color_attachments.data(),
                       (uint32_t)m_color_attachments.size());

        for (uint32_t index = 0; index < m_color_attachment_specs.size(); ++index) {
            bindTexture(is_multisampled, m_color_attachments[index]);
            // 根据附加格式进行添加
            switch (m_color_attachment_specs[index].TextureFormat) {
                case FramebufferTextureFormat::RGBA8:
                    attachColorTexture(m_color_attachments[index], m_specification.getSamples(),
                                       GL_RGBA8, GL_RGBA, m_specification.getWitdh(),
                                       m_specification.getHeight(), index);
                    break;
                case FramebufferTextureFormat::RED_INTEGER:
                    attachColorTexture(m_color_attachments[index], m_specification.getSamples(),
                                       GL_R32I, GL_RED_INTEGER, m_specification.getWitdh(),
                                       m_specification.getHeight(), index);
                    break;
            }
        }
    }

    // 创建深度纹理
    if (m_depth_attachment_spec.TextureFormat != FramebufferTextureFormat::None) {
        createTextures(is_multisampled, &m_depth_attachment, 1);
        bindTexture(is_multisampled, m_depth_attachment);
        switch (m_depth_attachment_spec.TextureFormat) {
            case FramebufferTextureFormat::DEPTH24_STENCIL8:
                attachDepthTexture(m_depth_attachment, m_specification.getSamples(),
                                   GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT,
                                   m_specification.getWitdh(), m_specification.getHeight());
                break;
        }
    }

    if (m_color_attachments.size() > 1) {
        HZ_CORE_ASSERT(m_color_attachments.size() <= 3, "当前最多支持四个color附件!");
        GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
                             GL_COLOR_ATTACHMENT3};
        glDrawBuffers((uint32_t)m_color_attachments.size(), buffers);
    } else {
        // 只绘制depth
        glDrawBuffer(GL_NONE);
    }

    // 检查 帧缓冲区的状态是否完整
    HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                   "Framebuffer is incomplete!");
    bindTexture(is_multisampled, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_rendrer_id);
    glViewport(0, 0, m_specification.getWitdh(), m_specification.getHeight());
}
void OpenGLFramebuffer::unBind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::resize(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0 || width > s_max_framebuffer_size ||
        height > s_max_framebuffer_size) {
        HZ_CORE_WARN("帧缓冲区接收了异常的调整大小: {0}, {1}", width, height);
        return;
    }
    m_specification.setSize(width, height);
    invalidate();
}

int OpenGLFramebuffer::readPixel(uint32_t attachment_index, int x, int y) const
{
    HZ_CORE_ASSERT(attachment_index < m_color_attachments.size(), "附加index越界!");
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
    int pixel_data;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel_data);
    return pixel_data;
}

void OpenGLFramebuffer::clearAttachment(uint32_t attachment_index, int value) const
{
    HZ_CORE_ASSERT(attachment_index < m_color_attachment_specs.size(), "附加index越界!");

    if (m_color_attachment_specs[attachment_index].TextureFormat ==
        FramebufferTextureFormat::RED_INTEGER) {
        glClearTexImage(m_color_attachments[attachment_index], 0, GL_RED_INTEGER, GL_INT, &value);
    }
}

}  // namespace Hazel