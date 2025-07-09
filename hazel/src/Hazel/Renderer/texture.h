#pragma once

/**
 * 纹理类型
 */

namespace Hazel
{

class HAZEL_API Texture
{
public:
    virtual ~Texture() = default;

    /// @brief 纹理的宽度
    virtual uint32_t getWidth() const = 0;

    /// @brief  纹理的高度
    virtual uint32_t getHeight() const = 0;

    /// @brief 用户上传纹理相关数据到shader中
    /// @param slot 上传哪个纹理插槽,默认为0
    virtual void bind(uint32_t slot = 0) const = 0;
};

class HAZEL_API Texture2D : public Texture
{
public:
    /// @brief 创建2d纹理类型
    /// @param path 图像路径(jpg/png)
    static Ref<Texture2D> create(const std::string& path);
};

}  // namespace Hazel