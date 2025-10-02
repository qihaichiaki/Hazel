#include "hzpch.h"
#include "buffer.h"
#include "renderer.h"

#include "PlatForm/OpenGL/opengl_buffer.h"

namespace Hazel
{

/////////// BufferLayer
/// @brief 返回shader使用数据类型的大小
static uint32_t shaderDataTypeSize(ShaderDataType type)
{
    switch (type) {
        case ShaderDataType::Float:
        case ShaderDataType::Int:
            return 4;
        case ShaderDataType::Float2:
        case ShaderDataType::Int2:
            return 8;
        case ShaderDataType::Float3:
        case ShaderDataType::Int3:
            return 12;
        case ShaderDataType::Float4:
        case ShaderDataType::Int4:
            return 16;
        case ShaderDataType::Mat3:
            return 4 * 3 * 3;
        case ShaderDataType::Mat4:
            return 4 * 4 * 4;
        case ShaderDataType::Bool:
            return 1;
    }

    HZ_CORE_ASSERT(false, "未知的ShaderData类型!");
    return 0;
}

BufferElement::BufferElement(ShaderDataType type, const std::string& name, bool normalized)
    : type{type}, name{name}, offset{0}, size{shaderDataTypeSize(type)}, normalized{normalized}
{
}

uint32_t BufferElement::getComponentCount() const
{
    switch (type) {
        case ShaderDataType::Float:
        case ShaderDataType::Int:
        case ShaderDataType::Bool:
            return 1;
        case ShaderDataType::Float2:
        case ShaderDataType::Int2:
            return 2;
        case ShaderDataType::Float3:
        case ShaderDataType::Int3:
            return 3;
        case ShaderDataType::Float4:
        case ShaderDataType::Int4:
            return 4;
        case ShaderDataType::Mat3:
            return 3 * 3;
        case ShaderDataType::Mat4:
            return 4 * 4;
    }

    HZ_CORE_ASSERT(false, "未知的ShaderData类型!");
    return 0;
}

BufferLayer::BufferLayer(const std::initializer_list<BufferElement>& elements)
    : m_elements{elements}
{
    calculateOffsetsAndStride();
}

void BufferLayer::calculateOffsetsAndStride()
{
    m_stride = 0;
    for (auto& element : m_elements) {
        element.offset = m_stride;
        m_stride += element.size;
    }
}

/////////// VertexBuffer
Ref<VertexBuffer> VertexBuffer::create(float* vertices, uint32_t size)
{
    switch (Renderer::getRendererAPI()) {
        case RendererAPI::API::None:
            HZ_CORE_ASSERT(false, "尚未支持的RendererAPI渲染");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<OpenGLVertexBuffer>(vertices, size);
    }

    HZ_CORE_ASSERT(false, "RendererAPI 未知选项!");
    return nullptr;
}

Ref<VertexBuffer> VertexBuffer::create(uint32_t size)
{
    switch (Renderer::getRendererAPI()) {
        case RendererAPI::API::None:
            HZ_CORE_ASSERT(false, "尚未支持的RendererAPI渲染");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<OpenGLVertexBuffer>(size);
    }

    HZ_CORE_ASSERT(false, "RendererAPI 未知选项!");
    return nullptr;
}

/////////// IndexBuffer
Ref<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t count)
{
    switch (Renderer::getRendererAPI()) {
        case RendererAPI::API::None:
            HZ_CORE_ASSERT(false, "尚未支持的RendererAPI渲染");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLIndexBuffer>(indices, count);
    }

    HZ_CORE_ASSERT(false, "RendererAPI 未知选项!");
    return nullptr;
}

}  // namespace Hazel