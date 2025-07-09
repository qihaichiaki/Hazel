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
            return std::make_shared<OpenGLTexture2D>(path);
    }

    HZ_CORE_ASSERT(false, "RendererAPI 未知选项!");
    return nullptr;
}
}  // namespace Hazel