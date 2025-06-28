#pragma once

#include "core.h"
#include "window.h"
#include "layer_stack.h"

namespace Hazel
{

class Event;
class WindowCloseEvent;
class Application
{
public:
    HAZEL_API Application();
    HAZEL_API virtual ~Application();

    HAZEL_API void run();

    HAZEL_API void pushLayer(Layer* layer);
    HAZEL_API void pushOverlay(Layer* overly);

private:
    void onEvent(Event&);

    bool onWindowClosed(WindowCloseEvent&);

private:
    std::unique_ptr<Window> m_window;
    bool m_running = true;

    LayerStack m_layer_stack;
};

Application* createApplication();

}  // namespace Hazel