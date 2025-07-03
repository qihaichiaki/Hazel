#include "hzpch.h"
#include "opengl_buffer.h"

#include <glad/glad.h>

namespace Hazel
{

////////////////////////////////////////////////////////////
// VertexBuffer ////////////////////////////////////////////

OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
{
    glCreateBuffers(1, &m_rendrer_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_rendrer_id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}
OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    glDeleteBuffers(1, &m_rendrer_id);
}

void OpenGLVertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_rendrer_id);
}

void OpenGLVertexBuffer::unBind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// IndexBuffer ////////////////////////////////////////////

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : m_count{count}
{
    glCreateBuffers(1, &m_rendrer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendrer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    glDeleteBuffers(1, &m_rendrer_id);
}

void OpenGLIndexBuffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendrer_id);
}

void OpenGLIndexBuffer::unBind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

uint32_t OpenGLIndexBuffer::getCount() const
{
    return m_count;
}
////////////////////////////////////////////////////////////

}  // namespace Hazel