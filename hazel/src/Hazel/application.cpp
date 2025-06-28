#include "hzpch.h"
#include "application.h"
#include "Hazel/Events/application_event.h"

#include <glad/glad.h>
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

        // 层级从左往右更新/渲染
        for (auto& layer : m_layer_stack) {
            layer->onUpdate();
        }

        m_window->onUpdate();
    }
}

void Application::onEvent(Event& event)
{
    EventDispatcher dispatcher{event};
    dispatcher.dispatch<WindowCloseEvent>(APP_BIND_FUNC(onWindowClosed));

    for (auto layer_it = m_layer_stack.end(); layer_it != m_layer_stack.begin();) {
        (*(--layer_it))->onEvent(event);
        if (event.isHandled()) {
            break;
        }
    }
}

bool Application::onWindowClosed(WindowCloseEvent&)
{
    m_running = false;
    return true;
}

void Application::pushLayer(Layer* layer)
{
    m_layer_stack.pushLayer(layer);
}

void Application::pushOverlay(Layer* overly)
{
    m_layer_stack.pushOverlay(overly);
}

}  // namespace Hazel
