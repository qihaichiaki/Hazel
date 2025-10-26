#pragma once

/**
 * shader能够固定获取的一块内存区域, 不需要每次shader进行上传uniform
 */

namespace Hazel
{

class HAZEL_API UniformBuffer
{
public:
    virtual ~UniformBuffer() {}
    virtual void setData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

    static Ref<UniformBuffer> create(uint32_t size, uint32_t binding);
};

}  // namespace Hazel