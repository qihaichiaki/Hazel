#include "hzpch.h"
#include "opengl_uniform_buffer.h"

#include <glad/glad.h>

namespace Hazel
{
OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
{
    glCreateBuffers(1, &m_renderer_id);
    glNamedBufferData(m_renderer_id, size, nullptr,
                      GL_DYNAMIC_DRAW);  // TODO: investigate usage hint
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_renderer_id);
}
OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
    glDeleteBuffers(1, &m_renderer_id);
}

void OpenGLUniformBuffer::setData(const void* data, uint32_t size, uint32_t offset)
{
    glNamedBufferSubData(m_renderer_id, offset, size, data);
}
}  // namespace Hazel