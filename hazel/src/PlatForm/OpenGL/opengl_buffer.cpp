#include "hzpch.h"
#include "opengl_buffer.h"

#include <glad/glad.h>

namespace Hazel
{

////////////////////////////////////////////////////////////
// VertexBuffer ////////////////////////////////////////////

OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
{
    HZ_PROFILE_FUNCTION();

    glCreateBuffers(1, &m_rendrer_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_rendrer_id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}
OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
{
    HZ_PROFILE_FUNCTION();

    glCreateBuffers(1, &m_rendrer_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_rendrer_id);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);  // 动态顶点数据
}
OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    HZ_PROFILE_FUNCTION();

    glDeleteBuffers(1, &m_rendrer_id);
}

void OpenGLVertexBuffer::bind() const
{
    HZ_PROFILE_FUNCTION();

    glBindBuffer(GL_ARRAY_BUFFER, m_rendrer_id);
}

void OpenGLVertexBuffer::unBind() const
{
    HZ_PROFILE_FUNCTION();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::setData(const void* data, uint32_t size)
{
    HZ_PROFILE_FUNCTION();

    glBindBuffer(GL_ARRAY_BUFFER, m_rendrer_id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// IndexBuffer ////////////////////////////////////////////

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : m_count{count}
{
    HZ_PROFILE_FUNCTION();

    glCreateBuffers(1, &m_rendrer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendrer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    HZ_PROFILE_FUNCTION();

    glDeleteBuffers(1, &m_rendrer_id);
}

void OpenGLIndexBuffer::bind() const
{
    HZ_PROFILE_FUNCTION();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendrer_id);
}

void OpenGLIndexBuffer::unBind() const
{
    HZ_PROFILE_FUNCTION();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
////////////////////////////////////////////////////////////

}  // namespace Hazel