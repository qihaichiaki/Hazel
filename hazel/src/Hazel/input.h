#pragma once

/**
 * 输入实例
 * 全局唯一, 需要不同平台进行实现相关接口
 */

#include "Hazel/core.h"

namespace Hazel
{

class HAZEL_API Input
{
public:
    /// @brief 当前按键是否按下
    static bool isKeyPressed(int keycode)
    {
        return s_instance->isKeyPressedImpl(keycode);
    }

    /// @brief 当前鼠标按键是否按下
    static bool isMouseButtonPressed(int button)
    {
        return s_instance->isMouseButtonPressedImpl(button);
    }

    /// @brief 获取当前鼠标坐标
    static std::pair<float, float> getMousePosition()
    {
        return s_instance->getMousePositionImpl();
    }

    /// @brief 获取当前鼠标X坐标
    static float getMouseX()
    {
        return s_instance->getMouseXImpl();
    }

    /// @brief 获取当前鼠标Y坐标
    static float getMouseY()
    {
        return s_instance->getMouseYImpl();
    }

protected:
    virtual bool isKeyPressedImpl(int) = 0;
    virtual bool isMouseButtonPressedImpl(int) = 0;
    virtual std::pair<float, float> getMousePositionImpl() = 0;
    virtual float getMouseXImpl() = 0;
    virtual float getMouseYImpl() = 0;

private:
    static Input* s_instance;
};

}  // namespace Hazel