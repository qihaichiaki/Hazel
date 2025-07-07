#include "hzpch.h"
#include "shader.h"

#include "renderer.h"
#include "PlatForm/OpenGL/opengl_shader.h"

namespace Hazel
{
Shader* Shader::create(const std::string& vertex_src, const std::string& fragment_src)
{
    switch (Renderer::getRendererAPI()) {
        case RendererAPI::API::None:
            HZ_CORE_ASSERT(false, "尚未支持的RendererAPI渲染");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLShader{vertex_src, fragment_src};
    }

    HZ_CORE_ASSERT(false, "RendererAPI 未知选项!");
    return nullptr;
}

}  // namespace Hazel