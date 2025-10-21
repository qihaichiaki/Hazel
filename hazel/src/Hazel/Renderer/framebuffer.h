#pragma once

/**
 * 帧缓冲区, 将直接渲染到窗口上的内容渲染到帧缓冲区中
 */

namespace Hazel
{

enum class HAZEL_API FramebufferTextureFormat {
    None,
    RGBA8,            // 颜色附件格式
    RED_INTEGER,      // int 类型附件格式, 方便读取id
    DEPTH24_STENCIL8  // 深度附件格式
};

struct HAZEL_API FramebufferTextureSpecification
{
    FramebufferTextureSpecification() = default;
    FramebufferTextureSpecification(FramebufferTextureFormat texture_format)
        : TextureFormat{texture_format}
    {
    }

    FramebufferTextureFormat TextureFormat;
    // TODO: filtering/ wrap
};

class FramebufferTextureAttachmentSpecification
{
public:
    HAZEL_API FramebufferTextureAttachmentSpecification() = default;
    HAZEL_API FramebufferTextureAttachmentSpecification(
        std::initializer_list<FramebufferTextureSpecification> attachments)
        : m_attachments{attachments}
    {
    }

    HAZEL_API std::vector<FramebufferTextureSpecification>& getAttachments()
    {
        return m_attachments;
    }
    HAZEL_API const std::vector<FramebufferTextureSpecification>& getAttachments() const
    {
        return m_attachments;
    }

private:
    std::vector<FramebufferTextureSpecification> m_attachments;
};

struct FramebufferSpecification
{
    HAZEL_API void setSize(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
    }
    HAZEL_API void setSamples(uint32_t samples)
    {
        m_samples = samples;
    }

    HAZEL_API uint32_t getWitdh() const
    {
        return m_width;
    }
    HAZEL_API uint32_t getHeight() const
    {
        return m_height;
    }
    HAZEL_API uint32_t getSamples() const
    {
        return m_samples;
    }

    HAZEL_API void setAttachmentSpecification(
        const FramebufferTextureAttachmentSpecification& attachment_specification)
    {
        m_attachment_specification = attachment_specification;
    }
    HAZEL_API const FramebufferTextureAttachmentSpecification& getAttachmentSpecification() const
    {
        return m_attachment_specification;
    }

private:
    uint32_t m_width = 0, m_height = 0;
    uint32_t m_samples = 1;  // 是否多重采样
    FramebufferTextureAttachmentSpecification m_attachment_specification;
};

class HAZEL_API Framebuffer
{
public:
    virtual ~Framebuffer() {}

    /// @brief 帧缓冲区绑定
    virtual void bind() const = 0;
    /// @brief 帧缓冲区解绑
    virtual void unBind() const = 0;

    /// @brief 重置帧缓冲区大小
    virtual void resize(uint32_t width, uint32_t height) = 0;

    /// @brief 获取帧缓冲区的颜色附加纹理的renderer id
    /// @param index 指定附加color附件的下标
    virtual uint32_t getColorAttachmentRendererID(uint32_t index = 0) const = 0;

    /// @brief 获取对应坐标的对应附加缓冲区内的值
    /// @param attachment_index int附件的下标
    /// @param x 坐标x
    /// @param y 坐标y
    /// @return 附件内的值int
    virtual int readPixel(uint32_t attachment_index, int x, int y) const = 0;

    /// @brief 获取当前帧缓冲区的信息
    virtual const FramebufferSpecification& getSpecification() const = 0;

    /// @brief 根据帧缓冲区规范创建帧缓冲区对象
    static Ref<Framebuffer> create(const FramebufferSpecification& spec);
};
}  // namespace Hazel