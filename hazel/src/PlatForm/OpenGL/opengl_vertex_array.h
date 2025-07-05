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

    void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertex_buffer) override;
    const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const override
    {
        return m_vertex_buffers;
    }
    void setIndexBuffer(const std::shared_ptr<IndexBuffer>& index_buffer) override;
    std::shared_ptr<IndexBuffer> getIndexBuffer() const override
    {
        return m_index_buffer;
    }

private:
    std::vector<std::shared_ptr<VertexBuffer>> m_vertex_buffers;
    std::shared_ptr<IndexBuffer> m_index_buffer;
    uint32_t m_vertex_array_id;
};

}  // namespace Hazel