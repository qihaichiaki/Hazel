#include "hzpch.h"
#include "opengl_vertex_array.h"

#include <glad/glad.h>

// shader type -> gl type
static GLenum shaderDataTypeToOpenGLBaseType(Hazel::ShaderDataType type)
{
    switch (type) {
        case Hazel::ShaderDataType::Float:
        case Hazel::ShaderDataType::Float2:
        case Hazel::ShaderDataType::Float3:
        case Hazel::ShaderDataType::Float4:
        case Hazel::ShaderDataType::Mat3:
        case Hazel::ShaderDataType::Mat4:
            return GL_FLOAT;
        case Hazel::ShaderDataType::Bool:
            return GL_BOOL;
        case Hazel::ShaderDataType::Int:
        case Hazel::ShaderDataType::Int2:
        case Hazel::ShaderDataType::Int3:
        case Hazel::ShaderDataType::Int4:
            return GL_INT;
    }

    HZ_CORE_ASSERT(false, "未知的ShaderData类型!");
    return 0;
}

namespace Hazel
{
OpenGLVertexArray::OpenGLVertexArray()
{
    HZ_PROFILE_FUNCTION();

    glCreateVertexArrays(1, &m_vertex_array_id);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
    HZ_PROFILE_FUNCTION();

    glDeleteVertexArrays(1, &m_vertex_array_id);
}

void OpenGLVertexArray::bind() const
{
    HZ_PROFILE_FUNCTION();

    glBindVertexArray(m_vertex_array_id);
}

void OpenGLVertexArray::unBind() const
{
    HZ_PROFILE_FUNCTION();

    glBindVertexArray(0);
}

void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertex_buffer)
{
    HZ_PROFILE_FUNCTION();

    auto& buffer_layer = vertex_buffer->getLayout();
    HZ_CORE_ASSERT(buffer_layer.getBufferElement().size(), "当前添加的顶点缓冲区未设置布局!");

    glBindVertexArray(m_vertex_array_id);
    vertex_buffer->bind();

    // 设置布局
    int index = 0;
    for (const auto& element : buffer_layer) {
        // 为shader启动顶点属性
        switch (element.type) {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, element.getComponentCount(),
                                      shaderDataTypeToOpenGLBaseType(element.type),
                                      element.normalized ? GL_TRUE : GL_FALSE,
                                      buffer_layer.getStride(),
                                      reinterpret_cast<const void*>((uint16_t)element.offset));
                break;
            case ShaderDataType::Bool:
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
                glEnableVertexAttribArray(index);
                glVertexAttribIPointer(index, element.getComponentCount(),
                                       shaderDataTypeToOpenGLBaseType(element.type),
                                       buffer_layer.getStride(),
                                       reinterpret_cast<const void*>((uint16_t)element.offset));
                break;
            case Hazel::ShaderDataType::Mat3:
            case Hazel::ShaderDataType::Mat4:
                // TODO: 对于mat3，mat4, 计算偏移得时候需要按照getComponentCount依次设置
                break;

            default:
                break;
        }

        index++;
    }

    m_vertex_buffers.push_back(vertex_buffer);
}

void OpenGLVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& index_buffer)
{
    HZ_PROFILE_FUNCTION();

    glBindVertexArray(m_vertex_array_id);
    index_buffer->bind();

    m_index_buffer = index_buffer;
}

}  // namespace Hazel
