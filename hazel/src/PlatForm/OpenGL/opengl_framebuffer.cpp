#include <hzpch.h>

#include "opengl_framebuffer.h"

#include <glad/glad.h>

namespace Hazel
{

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec) : m_specification(spec)
{
    invalidate();
}
OpenGLFramebuffer::~OpenGLFramebuffer()
{
    glDeleteFramebuffers(1, &m_rendrer_id);
}

void OpenGLFramebuffer::invalidate()
{
    // 创建帧缓冲区
    glCreateFramebuffers(1, &m_rendrer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_rendrer_id);
    // 创建颜色纹理
    glCreateTextures(GL_TEXTURE_2D, 1, &m_color_attachment);
    glBindTexture(GL_TEXTURE_2D, m_color_attachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_specification.Width, m_specification.Height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 贴图附件
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_attachment,
                           0);
    // 创建深度缓冲区
    glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_attachment);
    glBindTexture(GL_TEXTURE_2D, m_depth_attachment);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_specification.Width,
                   m_specification.Height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
                           m_depth_attachment, 0);
    // 检查 帧缓冲区的状态是否完整
    HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                   "Framebuffer is incomplete!");
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_rendrer_id);
}
void OpenGLFramebuffer::unBind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace Hazel