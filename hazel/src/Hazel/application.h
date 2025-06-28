#pragma once

#include "core.h"
#include "window.h"

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

private:
    void onEvent(Event&);

    bool onWindowClosed(WindowCloseEvent&);

private:
    std::unique_ptr<Window> m_window;
    bool m_running = true;
};

Application* createApplication();

}  // namespace Hazel