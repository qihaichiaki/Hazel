#include <hzpch.h>
#include "renderer.h"
#include "framebuffer.h"

#include <PlatForm/OpenGL/opengl_framebuffer.h>

namespace Hazel
{
Ref<Framebuffer> Framebuffer::create(const FramebufferSpecification& spec)
{
    switch (Renderer::getRendererAPI()) {
        case RendererAPI::API::None:
            HZ_CORE_ASSERT(false, "尚未支持的RendererAPI渲染");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<OpenGLFramebuffer>(spec);
    }

    HZ_CORE_ASSERT(false, "RendererAPI 未知选项!");
    return nullptr;
}
}  // namespace Hazel