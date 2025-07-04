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

    void setLayout(const BufferLayer& layout) override
    {
        m_layout = layout;
    }
    const BufferLayer& getLayout() const override
    {
        return m_layout;
    }

private:
    uint32_t m_rendrer_id;
    BufferLayer m_layout;
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