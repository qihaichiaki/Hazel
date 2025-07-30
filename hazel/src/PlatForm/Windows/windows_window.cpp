#include "hzpch.h"
#include "windows_window.h"

#include "Hazel/Events/application_event.h"
#include "Hazel/Events/key_event.h"
#include "Hazel/Events/mouse_event.h"

#include <PlatForm/OpenGL/opengl_context.h>

namespace Hazel
{
Scope<Window> Window::create(const WindowProps& props)
{
    return createScope<WindowsWindow>(props);
}

WindowsWindow::WindowsWindow(const WindowProps& props)
{
    HZ_PROFILE_FUNCTION();

    init(props);
}

WindowsWindow::~WindowsWindow()
{
    HZ_PROFILE_FUNCTION();

    shutdown();
}

static bool s_is_window_init = false;

static void glfwErrorCallBack(int error, const char* description)
{
    HZ_CORE_ERROR("glfw error ({0}): {1}", error, description);
}

void WindowsWindow::init(const WindowProps& props)
{
    HZ_PROFILE_FUNCTION();

    m_data.title = props.title;
    m_data.width = props.width;
    m_data.height = props.height;

    if (!s_is_window_init) {
        int is_init = 0;
        {
            HZ_PROFILE_SCOPE("glfwInit");
            is_init = glfwInit();
        }
        HZ_CORE_ASSERT(is_init, "glfwInit初始化失败!")

        glfwSetErrorCallback(glfwErrorCallBack);
        s_is_window_init = true;  // 窗口只初始化一次
    }
    // 通过glfw创建窗口
    {
        HZ_PROFILE_SCOPE("glfwCreateWindow");
        m_window =
            glfwCreateWindow(m_data.width, m_data.height, m_data.title.c_str(), nullptr, nullptr);
    }
    HZ_CORE_ASSERT(m_window, "glfwInit初始化失败!")

    m_context = std::make_unique<OpenGlContext>(m_window);
    m_context->init();

    // 设置用户字段, 方便在glfw触发事件时使用
    glfwSetWindowUserPointer(m_window, &m_data);

    setVSync(true);
    m_data.is_vsnc = true;

    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        WindowData& data = *((WindowData*)(glfwGetWindowUserPointer(window)));
        data.width = (unsigned int)width;
        data.height = (unsigned int)height;

        WindowResizeEvent event{(unsigned int)width, (unsigned int)height};
        data.event_call_back(event);
    });

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
        WindowData& data = *((WindowData*)(glfwGetWindowUserPointer(window)));

        WindowCloseEvent event{};
        data.event_call_back(event);
    });

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int, int action, int) {
        WindowData& data = *((WindowData*)(glfwGetWindowUserPointer(window)));

        switch (action) {
            case GLFW_PRESS:
            {
                KeyPressedEvent event{key, 0};
                data.event_call_back(event);
            } break;
            case GLFW_RELEASE:
            {
                KeyReleasedEvent event{key};
                data.event_call_back(event);
            } break;
            case GLFW_REPEAT:
            {
                KeyPressedEvent event{key, 1};
                data.event_call_back(event);
            } break;
            default:
                break;
        }
    });

    glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int codepoint) {
        WindowData& data = *((WindowData*)(glfwGetWindowUserPointer(window)));

        KeyTypedEvent event{(int)codepoint};
        data.event_call_back(event);
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int) {
        WindowData& data = *((WindowData*)(glfwGetWindowUserPointer(window)));

        switch (action) {
            case GLFW_PRESS:
            {
                MouseButtonPressedEvent event{button};
                data.event_call_back(event);
            } break;
            case GLFW_RELEASE:
            {
                MouseButtonReleasedEvent event{button};
                data.event_call_back(event);
            } break;
            default:
                break;
        }
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double x_offset, double y_offset) {
        WindowData& data = *((WindowData*)(glfwGetWindowUserPointer(window)));

        MouseScrolledEvent event{(float)x_offset, (float)y_offset};
        data.event_call_back(event);
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x_pos, double y_pos) {
        WindowData& data = *((WindowData*)(glfwGetWindowUserPointer(window)));

        MouseMovedEvent event{(float)x_pos, (float)y_pos};
        data.event_call_back(event);
    });
}

void WindowsWindow::shutdown()
{
    HZ_PROFILE_FUNCTION();

    glfwDestroyWindow(m_window);
    m_window = nullptr;
}

void WindowsWindow::onUpdate()
{
    HZ_PROFILE_FUNCTION();

    // glfw轮询事件
    glfwPollEvents();
    // 图形上下文交换
    m_context->swapBuffer();
}

void WindowsWindow::setVSync(bool enable)
{
    HZ_PROFILE_FUNCTION();

    if (enable) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }

    m_data.is_vsnc = enable;
}

}  // namespace Hazel
