#pragma once

#include "Hazel/Core/input.h"

namespace Hazel
{

class WindowsInput : public Input
{
protected:
    bool isKeyPressedImpl(int) override;
    bool isMouseButtonPressedImpl(int) override;
    std::pair<float, float> getMousePositionImpl() override;
    float getMouseXImpl() override;
    float getMouseYImpl() override;
};

}  // namespace Hazel