#include "hzpch.h"
#include "application.h"
#include "Hazel/Events/application_event.h"

#include "Hazel/input.h"
#include <glad/glad.h>
namespace Hazel
{
Application* Application::s_instance = nullptr;

Application::Application()
{
    HZ_CORE_ASSERT(s_instance == nullptr, "application被创建多次!")
    s_instance = this;

    m_window = std::unique_ptr<Window>(Window::create());
    m_window->setEventCallBack(HZ_BIND_EVENT_FN(Application::onEvent));
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
    dispatcher.dispatch<WindowCloseEvent>(HZ_BIND_EVENT_FN(Application::onWindowClosed));

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
    layer->onAttach();
    m_layer_stack.pushLayer(layer);
}

void Application::pushOverlay(Layer* overly)
{
    overly->onAttach();
    m_layer_stack.pushOverlay(overly);
}

void popLayer(Layer* layer)
{
    layer->onDetach();
    // TODO: layer pop待实现
}

void popOverlay(Layer* overly)
{
    overly->onDetach();
    // TODO: overly pop待实现
}

}  // namespace Hazel
