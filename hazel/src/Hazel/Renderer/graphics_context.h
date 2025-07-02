#pragma once

/**
 * 图形上下文抽象接口
 */

namespace Hazel
{
class GraphicsContext
{
public:
    virtual ~GraphicsContext() {}

    /// @brief 图形上下文初始化
    virtual void init() = 0;

    /// @brief 图形上下文交换缓冲区
    virtual void swapBuffer() = 0;
};
}  // namespace Hazel