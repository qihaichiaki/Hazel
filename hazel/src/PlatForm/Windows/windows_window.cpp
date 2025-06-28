#include "hzpch.h"
#include "windows_window.h"

namespace Hazel
{
Window* Window::create(const WindowProps& props)
{
    return new WindowsWindow{props};
}

WindowsWindow::WindowsWindow(const WindowProps& props)
{
    init(props);
}

WindowsWindow::~WindowsWindow()
{
    shutdown();
}

static bool s_is_window_init = false;

void WindowsWindow::init(const WindowProps& props)
{
    m_data.title = props.title;
    m_data.width = props.width;
    m_data.height = props.height;

    if (!s_is_window_init) {
        int is_init = glfwInit();
        HZ_CORE_ASSERT(is_init, "glfwInit初始化失败!")

        s_is_window_init = true;  // 窗口只初始化一次
    }
    // 通过glfw创建窗口
    m_window =
        glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);
    HZ_CORE_ASSERT(m_window, "glfwInit初始化失败!")
    // glfw创建当前的上下文
    glfwMakeContextCurrent(m_window);
    // 设置用户字段, 方便在glfw触发事件时使用
    glfwSetWindowUserPointer(m_window, &m_data);

    setVSync(true);
    m_data.is_vsnc = true;
}

void WindowsWindow::shutdown()
{
    glfwDestroyWindow(m_window);
    m_window = nullptr;
}

void WindowsWindow::onUpdate()
{
    // glfw轮询事件
    glfwPollEvents();
    // glfw交换缓冲区
    glfwSwapBuffers(m_window);
}

void WindowsWindow::setVSync(bool enable)
{
    if (enable) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }

    m_data.is_vsnc = enable;
}

}  // namespace Hazel
