#pragma once

/**
 * opengl特化顶点数组版本
 */

#include "Hazel/Renderer/vertex_array.h"

namespace Hazel
{

class OpenGLVertexArray : public VertexArray
{
public:
    OpenGLVertexArray();
    ~OpenGLVertexArray();

    void bind() const override;
    void unBind() const override;

    void addVertexBuffer(const Ref<VertexBuffer>& vertex_buffer) override;
    const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const override
    {
        return m_vertex_buffers;
    }
    void setIndexBuffer(const Ref<IndexBuffer>& index_buffer) override;
    Ref<IndexBuffer> getIndexBuffer() const override
    {
        return m_index_buffer;
    }

private:
    std::vector<Ref<VertexBuffer>> m_vertex_buffers;
    Ref<IndexBuffer> m_index_buffer;
    uint32_t m_vertex_array_id;
};

}  // namespace Hazel