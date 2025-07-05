#include "hzpch.h"
#include "opengl_renderer_api.h"

#include <glad/glad.h>

namespace Hazel
{
void OpenGLRendererAPI::setClearColor(const glm::vec4& clear_color)
{
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
}

void OpenGLRendererAPI::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::drawIndexed(const std::shared_ptr<VertexArray>& vertex_array)
{
    glDrawElements(GL_TRIANGLES, vertex_array->getIndexBuffer()->getCount(), GL_UNSIGNED_INT,
                   nullptr);
}

}  // namespace Hazel