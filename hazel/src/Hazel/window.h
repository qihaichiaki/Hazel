#pragma once

/**
 * 窗口抽象类, 适配多个不同的平台
 */

#include "hzpch.h"

#include "core.h"
#include "Events/event.h"

namespace Hazel
{

/// @brief 窗口的初始化选项
struct WindowProps
{
    std::string title;
    unsigned int width, height;

    WindowProps(const std::string& title = "Hazel Engine",
                unsigned int width = 1280,
                unsigned int height = 720)
        : title{title}, width{width}, height{height}
    {
    }
};

class HAZEL_API Window
{
public:
    using EventCallBackFn = std::function<void(Event&)>;

    virtual ~Window() {}

    /// @brief 窗口事件派发以及交换缓冲区
    virtual void onUpdate() = 0;
    /// @brief 窗口宽度
    virtual unsigned int getWidth() const = 0;
    /// @brief 窗口高度
    virtual unsigned int getHeight() const = 0;

    /// @brief 设置窗口事件处理回调
    virtual void setEventCallBack(EventCallBackFn call_back) = 0;
    /// @brief 是否启用垂直同步
    virtual void setVSync(bool enable) = 0;
    /// @brief 是否垂直同步
    virtual bool isVSync() const = 0;

    /// @brief 返回底层窗口句柄
    virtual void* getNativeWindow() const = 0;

public:
    /// @brief 创建窗口句柄
    /// @param props 窗口创建信息
    /// @return 相应平台的window示例
    static Window* create(const WindowProps& props = WindowProps{});
};
}  // namespace Hazel