#include "hzpch.h"
#include "renderer.h"

namespace Hazel
{

// TODO: 临时解决方案?
Renderer::SceneData* Renderer::m_scene_data = new Renderer::SceneData{};

void Renderer::beginScene(const OrthoGraphicCamera& camera)
{
    m_scene_data->projection_view_matrix = camera.getProjectionViewMatrix();
}

void Renderer::submit(const std::shared_ptr<Shader>& shader,
                      const std::shared_ptr<VertexArray>& vertex_array,
                      const glm::mat4& transform)
{
    shader->bind();
    // TODO: 硬编码
    shader->uploadUniformMat4("u_ProjectionView", m_scene_data->projection_view_matrix);
    shader->uploadUniformMat4("u_Transform", transform);
    vertex_array->bind();
    RendererCommand::drawIndexed(vertex_array);
}

void Renderer::endScene() {}

}  // namespace Hazel