#pragma once

/**
 * 帧缓冲区, 将直接渲染到窗口上的内容渲染到帧缓冲区中
 */

namespace Hazel
{

struct HAZEL_API FramebufferSpecification
{
    uint32_t Width, Height;
};

class HAZEL_API Framebuffer
{
public:
    /// @brief 帧缓冲区绑定
    virtual void bind() const = 0;
    /// @brief 帧缓冲区解绑
    virtual void unBind() const = 0;

    /// @brief 获取帧缓冲区的颜色附加纹理的renderer id
    virtual uint32_t getColorAttachmentRendererID() const = 0;

    /// @brief 根据帧缓冲区规范创建帧缓冲区对象
    static Ref<Framebuffer> create(const FramebufferSpecification& spec);
};
}  // namespace Hazel