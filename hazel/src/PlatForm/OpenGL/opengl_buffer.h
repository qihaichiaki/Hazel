#pragma once

#include "Hazel/Renderer/buffer.h"

namespace Hazel
{
class OpenGLVertexBuffer : public VertexBuffer
{
public:
    OpenGLVertexBuffer(float* vertices, uint32_t size);
    ~OpenGLVertexBuffer();

    void bind() const override;
    void unBind() const override;

private:
    uint32_t m_rendrer_id;
};

class OpenGLIndexBuffer : public IndexBuffer
{
public:
    OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
    ~OpenGLIndexBuffer();

    void bind() const override;
    void unBind() const override;
    uint32_t getCount() const override;

private:
    uint32_t m_rendrer_id;
    uint32_t m_count;
};
}  // namespace Hazel