#include "hzpch.h"
#include "renderer.h"

namespace Hazel
{

void Renderer::beginScene() {}

void Renderer::submit(const std::shared_ptr<VertexArray>& vertex_array)
{
    vertex_array->bind();
    RendererCommand::drawIndexed(vertex_array);
}

void Renderer::endScene() {}

}  // namespace Hazel