#include "hzpch.h"
#include "renderer.h"

#include "renderer_2d.h"

// TODO: 临时转换
#include "PlatForm/OpenGL/opengl_shader.h"

namespace Hazel
{

// TODO: 临时解决方案?
Renderer::SceneData* Renderer::m_scene_data = new Renderer::SceneData{};

void Renderer::init()
{
    HZ_PROFILE_FUNCTION();

    RendererCommand::init();
    // TODO: renderer初始化时带上2d渲染器初始化, 但是没有手动shutdown
    Renderer2D::init();
}

void Renderer::beginScene(const OrthoGraphicCamera& camera)
{
    m_scene_data->projection_view_matrix = camera.getProjectionViewMatrix();
}

void Renderer::submit(const Ref<Shader>& shader,
                      const Ref<VertexArray>& vertex_array,
                      const glm::mat4& transform)
{
    shader->bind();
    // TODO: 硬编码
    std::static_pointer_cast<OpenGLShader>(shader)->uploadUniformMat4(
        "u_ProjectionView", m_scene_data->projection_view_matrix);
    std::static_pointer_cast<OpenGLShader>(shader)->uploadUniformMat4("u_Transform", transform);
    vertex_array->bind();
    RendererCommand::drawIndexed(vertex_array, vertex_array->getIndexBuffer()->getCount());
}

void Renderer::endScene() {}

void Renderer::onWindowResize(uint32_t width, uint32_t height)
{
    RendererCommand::setViewport(width, height);
}

}  // namespace Hazel