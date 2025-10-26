#pragma once

#include "Hazel/Renderer/uniform_buffer.h"

namespace Hazel
{
class OpenGLUniformBuffer : public UniformBuffer
{
public:
    OpenGLUniformBuffer(uint32_t size, uint32_t binding);
    ~OpenGLUniformBuffer();

    void setData(const void* data, uint32_t size, uint32_t offset = 0) override;

private:
    uint32_t m_renderer_id{0};
};
}  // namespace Hazel