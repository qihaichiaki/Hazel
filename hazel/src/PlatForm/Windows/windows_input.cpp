#include "hzpch.h"
#include <Hazel/Core/input.h>

#include "Hazel/Core/application.h"
#include <GLFW/glfw3.h>

#ifdef HZ_PLATFORM_WINDOWS

namespace Hazel
{

bool Input::isKeyPressed(KeyCode keycode)
{
    auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
    int state = glfwGetKey(window, (int)keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::isMouseButtonPressed(KeyCode button)
{
    auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
    int state = glfwGetMouseButton(window, (int)button);
    return state == GLFW_PRESS;
}

std::pair<float, float> Input::getMousePosition()
{
    auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
    double x_pos, y_pos;
    glfwGetCursorPos(window, &x_pos, &y_pos);
    return {(float)x_pos, (float)y_pos};
}

float Input::getMouseX()
{
    auto [x, y] = getMousePosition();
    return x;
}

float Input::getMouseY()
{
    auto [x, y] = getMousePosition();
    return y;
}
}  // namespace Hazel

#endif