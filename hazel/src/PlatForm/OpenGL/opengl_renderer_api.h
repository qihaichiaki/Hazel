#pragma once

/**
 * 渲染api物理接口的opengl特化版本
 */

#include "hazel/Renderer/renderer_api.h"

namespace Hazel
{

class OpenGLRendererAPI : public RendererAPI
{
public:
    void setClearColor(const glm::vec4& clear_color) override;
    void clear() override;
    void drawIndexed(const std::shared_ptr<VertexArray>& vertex_array) override;
};
}  // namespace Hazel