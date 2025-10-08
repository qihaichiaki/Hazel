#pragma once

#include "base.h"
#include "window.h"
#include "layer_stack.h"

namespace Hazel
{

class Event;
class WindowCloseEvent;
class WindowResizeEvent;
class ImGuiLayer;
class Time;
class Application
{
public:
    HAZEL_API Application(const std::string& main_window_name = "Hazel引擎");
    HAZEL_API virtual ~Application();

    HAZEL_API void run();

    HAZEL_API void pushLayer(Layer* layer);
    HAZEL_API void pushOverlay(Layer* overly);
    HAZEL_API void popLayer(Layer* layer);
    HAZEL_API void popOverlay(Layer* overly);

    HAZEL_API void close()
    {
        m_running = false;
    }

    HAZEL_API Window& getWindow()
    {
        return (*m_window);
    }

    // TODO: 临时
    HAZEL_API ImGuiLayer* getImGuiLayer()
    {
        return m_imgui_layer;
    }

    // TODO: 临时实现类似单例的模式
    HAZEL_API static Application& get()
    {
        return (*s_instance);
    }

private:
    void onEvent(Event&);

    bool onWindowClosed(WindowCloseEvent&);
    bool onWindowResized(WindowResizeEvent&);

private:
    Scope<Window> m_window;
    ImGuiLayer* m_imgui_layer;  // 单独出现一层级
    bool m_running = true;
    bool m_minimized = false;
    LayerStack m_layer_stack;

    float m_last_frame_time = 0.0f;  // 最后一帧的时间

private:
    static Application* s_instance;
};

Application* createApplication();

}  // namespace Hazel