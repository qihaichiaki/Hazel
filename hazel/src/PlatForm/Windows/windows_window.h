#pragma once

#include "Hazel/window.h"
#include <GLFW/glfw3.h>

namespace Hazel
{

class WindowsWindow : public Window
{
public:
    WindowsWindow(const WindowProps& props);
    ~WindowsWindow();

    void onUpdate() override;
    unsigned int getWidth() const override
    {
        return m_data.width;
    }
    unsigned int getHeight() const override
    {
        return m_data.height;
    }

    void setEventCallBack(EventCallBackFn call_back) override
    {
        m_data.event_call_back = call_back;
    }
    void setVSync(bool enable) override;
    bool isVSync() const override
    {
        return m_data.is_vsnc;
    }

private:
    void init(const WindowProps&);
    void shutdown();

private:
    GLFWwindow* m_window;  // glfw窗口句柄

    struct WindowData
    {
        std::string title;
        unsigned int width, height;
        bool is_vsnc;
        EventCallBackFn event_call_back;
    };

    WindowData m_data;  // 窗口运行时数据
};

}  // namespace Hazel