#pragma once

namespace Hazel
{

/// @brief 顶点缓冲区接口
class VertexBuffer
{
public:
    virtual ~VertexBuffer() {}

    /// @brief 当前上下文绑定顶点缓冲区
    virtual void bind() const = 0;

    /// @brief 当前上下文解绑顶点缓冲区
    virtual void unBind() const = 0;

public:
    /// @brief 创建顶点缓冲区对象
    /// @param vertices 顶点们
    /// @param size 顶点缓冲区大小
    /// @return 顶点缓冲区对象, new, 注意防止内存泄漏
    static VertexBuffer* create(float* vertices, uint32_t size);
};

/// @brief 索引缓冲区接口
class IndexBuffer
{
public:
    virtual ~IndexBuffer() {}

    /// @brief 当前上下文绑定索引缓冲区
    virtual void bind() const = 0;

    /// @brief 当前上下文解绑索引缓冲区
    virtual void unBind() const = 0;

    /// @brief 当前索引个数
    virtual uint32_t getCount() const = 0;

public:
    /// @brief 创建索引缓冲区对象
    /// @param indices 索引们
    /// @param count 索引个数
    /// @return 索引缓冲区对象, new, 注意防止内存泄漏
    static IndexBuffer* create(uint32_t* indices, uint32_t count);
};
}  // namespace Hazel