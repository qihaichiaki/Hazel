#include "hzpch.h"
#include "application.h"
#include "Hazel/Events/application_event.h"
#include "Hazel/ImGui/imgui_layer.h"

#include "Hazel/Core/timestep.h"
// TODO: time使用, 临时方案
#include <GLFW/glfw3.h>

namespace Hazel
{
Application* Application::s_instance = nullptr;

Application::Application()
{
    HZ_CORE_ASSERT(s_instance == nullptr, "application被创建多次!")
    s_instance = this;

    m_window = std::unique_ptr<Window>(Window::create());
    m_window->setEventCallBack(HZ_BIND_EVENT_FN(Application::onEvent));

    m_imgui_layer = new ImGuiLayer{};
    pushOverlay(m_imgui_layer);
}

Application::~Application() {}

void Application::run()
{
    while (m_running) {
        float time = static_cast<float>(glfwGetTime());
        Timestep timestep = time - m_last_frame_time;
        m_last_frame_time = time;

        // 层级从左往右更新/渲染
        for (auto& layer : m_layer_stack) {
            layer->onUpdate(timestep);
        }
        // imgui 更新层
        m_imgui_layer->begin();
        for (auto& layer : m_layer_stack) {
            layer->onImGuiRender();
        }
        m_imgui_layer->end();

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
    m_layer_stack.pushLayer(layer);
}

void Application::pushOverlay(Layer* overly)
{
    m_layer_stack.pushOverlay(overly);
}

void Application::popLayer(Layer* layer)
{
    m_layer_stack.popLayer(layer);
}

void Application::popOverlay(Layer* overly)
{
    m_layer_stack.popOverlay(overly);
}
}  // namespace Hazel
