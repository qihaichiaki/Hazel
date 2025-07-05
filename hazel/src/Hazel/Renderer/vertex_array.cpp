#include "hzpch.h"
#include "vertex_array.h"
#include "renderer.h"

#include "PlatForm/OpenGL/opengl_vertex_array.h"

namespace Hazel
{

VertexArray* VertexArray::create()
{
    switch (Renderer::getRendererAPI()) {
        case RendererAPI::API::None:
            HZ_CORE_ASSERT(false, "尚未支持的RendererAPI渲染");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLVertexArray{};
    }

    HZ_CORE_ASSERT(false, "RendererAPI 未知选项!");
    return nullptr;
}

}  // namespace Hazel