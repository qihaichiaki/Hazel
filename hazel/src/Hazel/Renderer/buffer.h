#pragma once

namespace Hazel
{

/// @brief shader使用数据的类型
enum class ShaderDataType : uint8_t {
    None,
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Int2,
    Int3,
    Int4,
    Mat3,
    Mat4,
    Bool
};

struct BufferElement
{
    ShaderDataType type;
    std::string name;
    uint32_t offset;
    uint32_t size;
    bool normalized;

    HAZEL_API BufferElement(ShaderDataType type, const std::string& name, bool normalized = false);

    /// @brief 返回当前数据类型的个数是多少
    HAZEL_API uint32_t getComponentCount() const;
};

/// @brief 缓冲区布局
class BufferLayer
{
public:
    HAZEL_API BufferLayer() {}
    HAZEL_API BufferLayer(const std::initializer_list<BufferElement>& elements);

    HAZEL_API const std::vector<BufferElement>& getBufferElement() const
    {
        return m_elements;
    }
    HAZEL_API std::vector<BufferElement>::iterator begin()
    {
        return m_elements.begin();
    }
    HAZEL_API std::vector<BufferElement>::iterator end()
    {
        return m_elements.end();
    }
    HAZEL_API std::vector<BufferElement>::const_iterator begin() const
    {
        return m_elements.begin();
    }
    HAZEL_API std::vector<BufferElement>::const_iterator end() const
    {
        return m_elements.end();
    }

    /// @brief 返回当前元素布局的总偏移量
    HAZEL_API uint32_t getStride() const
    {
        return m_stride;
    }

private:
    /// @brief 计算每个元素的offset
    void calculateOffsetsAndStride();

private:
    std::vector<BufferElement> m_elements;  // 当前缓冲区布局的元素们
    uint32_t m_stride;                      // 当前缓冲区布局所有元素的总偏移量
};

/// @brief 顶点缓冲区接口
class HAZEL_API VertexBuffer
{
public:
    virtual ~VertexBuffer() {}

    /// @brief 当前上下文绑定顶点缓冲区
    virtual void bind() const = 0;

    /// @brief 当前上下文解绑顶点缓冲区
    virtual void unBind() const = 0;

    /// @brief 往动态顶点缓冲区对象内填充数据
    /// @param data 顶点数据
    /// @param size 顶点数据大小
    virtual void setData(const void* data, uint32_t size) = 0;

    /// @brief 设置顶点缓冲区布局
    virtual void setLayout(const BufferLayer& layout) = 0;

    /// @brief 获取顶点缓冲区布局
    virtual const BufferLayer& getLayout() const = 0;

public:
    /// @brief 创建静态顶点缓冲区对象
    /// @param vertices 顶点们
    /// @param size 顶点缓冲区大小
    /// @return 顶点缓冲区对象
    static Ref<VertexBuffer> create(float* vertices, uint32_t size);

    /// @brief 创建动态顶点缓冲区对象
    /// @param size 顶点缓冲区大小
    /// @return 顶点缓冲区对象
    static Ref<VertexBuffer> create(uint32_t size);
};

/// @brief 索引缓冲区接口
class HAZEL_API IndexBuffer
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
    static Ref<IndexBuffer> create(uint32_t* indices, uint32_t count);
};
}  // namespace Hazel