#pragma once

/**
 * 输入实例
 * 全局唯一, 需要不同平台进行实现相关接口
 */

#include "Hazel/Core/base.h"
#include "Hazel/Core/input_codes.h"

namespace Hazel
{

class HAZEL_API Input
{
public:
    /// @brief 当前按键是否按下
    static bool isKeyPressed(KeyCode keycode);
    /// @brief 当前鼠标按键是否按下
    static bool isMouseButtonPressed(KeyCode button);
    /// @brief 获取当前鼠标坐标
    static std::pair<float, float> getMousePosition();
    /// @brief 获取当前鼠标X坐标
    static float getMouseX();
    /// @brief 获取当前鼠标Y坐标
    static float getMouseY();
};

}  // namespace Hazel