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
    static bool isKeyPressed(int keycode)
    {
        return s_instance->isKeyPressedImpl(keycode);
    }

    static bool isMouseButtonPressed(int button)
    {
        return s_instance->isMouseButtonPressedImpl(button);
    }

    static std::pair<float, float> getMousePosition()
    {
        return s_instance->getMousePositionImpl();
    }

    static float getMouseX()
    {
        return s_instance->getMouseXImpl();
    }

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