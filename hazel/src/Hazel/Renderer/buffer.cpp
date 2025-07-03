#include "hzpch.h"
#include "buffer.h"
#include "renderer.h"

#include "PlatForm/OpenGL/opengl_buffer.h"

namespace Hazel
{

VertexBuffer* VertexBuffer::create(float* vertices, uint32_t size)
{
    switch (Renderer::getRendererAPI()) {
        case RendererAPI::None:
            HZ_CORE_ASSERT(false, "尚未支持的RendererAPI渲染");
            return nullptr;
        case RendererAPI::OpenGL:
            return new OpenGLVertexBuffer{vertices, size};
    }

    HZ_CORE_ASSERT(false, "RendererAPI 未知选项!");
    return nullptr;
}

IndexBuffer* IndexBuffer::create(uint32_t* indices, uint32_t count)
{
    switch (Renderer::getRendererAPI()) {
        case RendererAPI::None:
            HZ_CORE_ASSERT(false, "尚未支持的RendererAPI渲染");
            return nullptr;
        case RendererAPI::OpenGL:
            return new OpenGLIndexBuffer{indices, count};
    }

    HZ_CORE_ASSERT(false, "RendererAPI 未知选项!");
    return nullptr;
}

}  // namespace Hazel