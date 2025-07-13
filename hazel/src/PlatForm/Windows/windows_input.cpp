#include "hzpch.h"
#include "windows_input.h"

#include "Hazel/Core/application.h"
#include <GLFW/glfw3.h>

namespace Hazel
{
// TODO: 临时input 全局初始化
Input* Input::s_instance = new WindowsInput{};

bool WindowsInput::isKeyPressedImpl(int keycode)
{
    auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
    int state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::isMouseButtonPressedImpl(int button)
{
    auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
    int state = glfwGetMouseButton(window, button);
    return state == GLFW_PRESS;
}

std::pair<float, float> WindowsInput::getMousePositionImpl()
{
    auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
    double x_pos, y_pos;
    glfwGetCursorPos(window, &x_pos, &y_pos);
    return {(float)x_pos, (float)y_pos};
}

float WindowsInput::getMouseXImpl()
{
    auto [x, y] = getMousePositionImpl();
    return x;
}

float WindowsInput::getMouseYImpl()
{
    auto [x, y] = getMousePositionImpl();
    return y;
}
}  // namespace Hazel