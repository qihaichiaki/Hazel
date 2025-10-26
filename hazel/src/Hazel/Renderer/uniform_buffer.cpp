#include <hzpch.h>
#include "renderer.h"
#include "uniform_buffer.h"

#include <PlatForm/OpenGL/opengl_uniform_buffer.h>

namespace Hazel
{

Ref<UniformBuffer> UniformBuffer::create(uint32_t size, uint32_t binding)
{
    switch (Renderer::getRendererAPI()) {
        case RendererAPI::API::None:
            HZ_CORE_ASSERT(false, "尚未支持的RendererAPI渲染");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<OpenGLUniformBuffer>(size, binding);
    }

    HZ_CORE_ASSERT(false, "RendererAPI 未知选项!");
    return nullptr;
}
}  // namespace Hazel