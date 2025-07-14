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
    void init() override;
    void setClearColor(const glm::vec4& clear_color) override;
    void clear() override;
    void drawIndexed(const Ref<VertexArray>& vertex_array) override;
    void setViewport(uint32_t width, uint32_t height) override;
};
}  // namespace Hazel