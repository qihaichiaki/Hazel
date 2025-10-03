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

    /// @brief 设置纹理数据上传到gpu上
    /// @param data 纹理数据, 注意宽度和高度需要和当前纹理一致
    /// @param size 当前的纹理数据字节大小
    virtual void setData(void* data, uint32_t size) = 0;

    /// @brief 判断两个纹理是否一致
    virtual bool isEqual(const Texture&) = 0;
};

class HAZEL_API Texture2D : public Texture
{
public:
    /// @brief 创建2d纹理类型
    /// @param path 图像路径(jpg/png)
    static Ref<Texture2D> create(const std::string& path);

    /// @brief 创建2d纹理类型
    /// @param width 纹理宽度
    /// @param height 纹理高度
    /// @note 创建完毕后, 通关纹理的设置数据即可, 注意数据大小要和宽度高度匹配, 默认单位为RGBA
    static Ref<Texture2D> create(uint32_t width, uint32_t height);
};

}  // namespace Hazel