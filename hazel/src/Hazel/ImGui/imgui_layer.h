#pragma once

/**
 * imgui layer的实现
 */

#include "hazel/layer.h"

namespace Hazel
{

// 事件类型声明
class MouseButtonPressedEvent;
class MouseButtonReleasedEvent;
class MouseMovedEvent;
class MouseScrolledEvent;
class KeyPressedEvent;
class KeyReleasedEvent;
class KeyTypedEvent;
class WindowResizeEvent;

class ImGuiLayer : public Layer
{
public:
    HAZEL_API ImGuiLayer();
    HAZEL_API ~ImGuiLayer();

    HAZEL_API void onAttach() override;
    HAZEL_API void onDetach() override;
    HAZEL_API void onUpdate() override;
    HAZEL_API void onEvent(Event&) override;

private:
    /// @brief 鼠标按钮按下事件
    bool onMouseButtonPressedEvent(MouseButtonPressedEvent&);
    /// @brief 鼠标按钮松开事件
    bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent&);
    /// @brief 鼠标移动事件
    bool onMouseMovedEvent(MouseMovedEvent&);
    /// @brief 鼠标滑轮滚动事件
    bool onMouseScrolledEvent(MouseScrolledEvent&);
    /// @brief 按键按下事件
    bool onKeyPressedEvent(KeyPressedEvent&);
    /// @brief 按键松开事件
    bool onKeyReleasedEvent(KeyReleasedEvent&);
    /// @brief 文本输入事件
    bool onKeyTypedEvent(KeyTypedEvent&);
    /// @brief 窗口大小事件
    bool onWindowResizeEvent(WindowResizeEvent&);

private:
    float m_time;
};

}  // namespace Hazel