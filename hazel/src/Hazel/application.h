#pragma once

#include "core.h"
#include "window.h"

namespace Hazel
{

class Application
{
public:
    HAZEL_API Application();
    HAZEL_API virtual ~Application();

    HAZEL_API void run();

private:
    std::unique_ptr<Window> m_window;
    bool m_running = true;
};

Application* createApplication();

}  // namespace Hazel