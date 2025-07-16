#include "hzpch.h"
#include "texture.h"

#include "renderer.h"
#include "PlatForm/OpenGL/opengl_texture2d.h"

namespace Hazel
{

Ref<Texture2D> Texture2D::create(const std::string& path)
{
    switch (Renderer::getRendererAPI()) {
        case RendererAPI::API::None:
            HZ_CORE_ASSERT(false, "尚未支持的RendererAPI渲染");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<OpenGLTexture2D>(path);
    }

    HZ_CORE_ASSERT(false, "RendererAPI 未知选项!");
    return nullptr;
}

Ref<Texture2D> Texture2D::create(uint32_t width, uint32_t height)
{
    switch (Renderer::getRendererAPI()) {
        case RendererAPI::API::None:
            HZ_CORE_ASSERT(false, "尚未支持的RendererAPI渲染");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return createRef<OpenGLTexture2D>(width, height);
    }

    HZ_CORE_ASSERT(false, "RendererAPI 未知选项!");
    return nullptr;
}
}  // namespace Hazel