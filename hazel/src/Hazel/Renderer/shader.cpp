#include "hzpch.h"
#include "shader.h"

#include "renderer.h"
#include "PlatForm/OpenGL/opengl_shader.h"

namespace Hazel
{
Ref<Shader> Shader::create(const std::string& name,
                           const std::string& vertex_src,
                           const std::string& fragment_src)
{
    switch (Renderer::getRendererAPI()) {
        case RendererAPI::API::None:
            HZ_CORE_ASSERT(false, "尚未支持的RendererAPI渲染");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLShader>(name, vertex_src, fragment_src);
    }

    HZ_CORE_ASSERT(false, "RendererAPI 未知选项!");
    return nullptr;
}

Ref<Shader> Shader::create(const std::string& file_path)
{
    switch (Renderer::getRendererAPI()) {
        case RendererAPI::API::None:
            HZ_CORE_ASSERT(false, "尚未支持的RendererAPI渲染");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLShader>(file_path);
    }

    HZ_CORE_ASSERT(false, "RendererAPI 未知选项!");
    return nullptr;
}

//////////////////// ShaderLibrary

bool ShaderLibrary::isExists(const std::string& name) const
{
    return m_shaders.find(name) != m_shaders.end();
}

void ShaderLibrary::add(const std::string& name, const Ref<Shader>& shader)
{
    HZ_CORE_ASSERT(!isExists(name), "当前添加的{}-shader已经存在!", name);
    m_shaders[name] = shader;
}

void ShaderLibrary::add(const Ref<Shader>& shader)
{
    auto& shader_name = shader->getName();
    add(shader_name, shader);
}

Ref<Shader> ShaderLibrary::load(const std::string& name, const std::string& file_path)
{
    auto shader = Shader::create(file_path);
    add(name, shader);
    return shader;
}

Ref<Shader> ShaderLibrary::load(const std::string& file_path)
{
    auto shader = Shader::create(file_path);
    add(shader);
    return shader;
}

Ref<Shader> ShaderLibrary::get(const std::string& name) const
{
    HZ_CORE_ASSERT(isExists(name), "{}-shader并不存在!", name);
    return m_shaders.at(name);
}

}  // namespace Hazel