#include "hzpch.h"
#include "application.h"
#include "Hazel/Events/application_event.h"

#include <GLFW/glfw3.h>
namespace Hazel
{
#define APP_BIND_FUNC(x) std::bind(&Application::x, this, std::placeholders::_1)

Application::Application()
{
    m_window = std::unique_ptr<Window>(Window::create());
    m_window->setEventCallBack(APP_BIND_FUNC(onEvent));
}
Application::~Application() {}

void Application::run()
{
    while (m_running) {
        glClearColor(1, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        m_window->onUpdate();
    }
}

void Application::onEvent(Event& event)
{
    HZ_CORE_TRACE(event.toString());

    EventDispatcher dispatcher{event};
    dispatcher.dispatch<WindowCloseEvent>(APP_BIND_FUNC(onWindowClosed));
}

bool Application::onWindowClosed(WindowCloseEvent&)
{
    m_running = false;
    return true;
}

}  // namespace Hazel
