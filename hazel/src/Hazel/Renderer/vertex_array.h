#pragma once

/**
 * 抽象顶点数组
 * 用来作为顶点系列的标识, 方便将顶点缓冲区, 索引缓冲区绑定于此
 */

#include "Hazel/Renderer/buffer.h"

namespace Hazel
{

class HAZEL_API VertexArray
{
public:
    virtual ~VertexArray() {}

    /// @brief 顶点数组绑定
    virtual void bind() const = 0;

    /// @brief 顶点数组解绑
    virtual void unBind() const = 0;

    /// @brief 顶点数组中增加顶点缓冲区
    /// @warning VertexBuffer应该在添加之前设置布局, 否则不会生效
    /// @note 如果添加时没有setLayout会触发断言
    virtual void addVertexBuffer(const Ref<VertexBuffer>& vertex_buffer) = 0;

    /// @brief 返回当前顶点数组中的所有顶点缓冲区
    virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const = 0;

    /// @brief 为顶点数组设置索引缓冲区
    virtual void setIndexBuffer(const Ref<IndexBuffer>& index_buffer) = 0;

    /// @brief 返回顶点数组的索引缓冲区
    virtual Ref<IndexBuffer> getIndexBuffer() const = 0;

public:
    /// @brief 创建顶点数组对象
    /// @note 创建时会根据RenderApi选择创建适合当前的对象
    static Ref<VertexArray> create();
};
}  // namespace Hazel