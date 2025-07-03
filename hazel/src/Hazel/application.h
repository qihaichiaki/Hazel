#pragma once

#include "core.h"
#include "window.h"
#include "layer_stack.h"

#include "Hazel/Renderer/shader.h"

namespace Hazel
{

class Event;
class WindowCloseEvent;
class ImGuiLayer;
class Application
{
public:
    HAZEL_API Application();
    HAZEL_API virtual ~Application();

    HAZEL_API void run();

    HAZEL_API void pushLayer(Layer* layer);
    HAZEL_API void pushOverlay(Layer* overly);
    HAZEL_API void popLayer(Layer* layer);
    HAZEL_API void popOverlay(Layer* overly);

    HAZEL_API Window& getWindow()
    {
        return (*m_window);
    }

    // TODO: 临时实现类似单例的模式
    HAZEL_API static Application& get()
    {
        return (*s_instance);
    }

private:
    void onEvent(Event&);

    bool onWindowClosed(WindowCloseEvent&);

private:
    std::unique_ptr<Window> m_window;
    ImGuiLayer* m_imgui_layer;  // 单独出现一层级
    std::unique_ptr<Shader> m_shader;

    bool m_running = true;
    LayerStack m_layer_stack;

    static Application* s_instance;
};

Application* createApplication();

}  // namespace Hazel