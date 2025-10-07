#include "hzpch.h"
#include "application.h"
#include "Hazel/Events/application_event.h"
#include "Hazel/ImGui/imgui_layer.h"

#include "Hazel/Core/timestep.h"
#include "Hazel/Renderer/renderer.h"
// TODO: time使用, 临时方案
#include <GLFW/glfw3.h>

namespace Hazel
{
Application* Application::s_instance = nullptr;

Application::Application(const std::string& main_window_name)
{
    HZ_PROFILE_FUNCTION();

    HZ_CORE_ASSERT(s_instance == nullptr, "application被创建多次!")
    s_instance = this;

    m_window = Window::create(WindowProps{main_window_name});
    m_window->setEventCallBack(HZ_BIND_EVENT_FN(Application::onEvent));

    Renderer::init();

    m_imgui_layer = new ImGuiLayer{};
    pushOverlay(m_imgui_layer);
}

Application::~Application() {}

void Application::run()
{
    HZ_PROFILE_FUNCTION();

    while (m_running) {
        HZ_PROFILE_SCOPE("RunLoop");

        float time = static_cast<float>(glfwGetTime());
        Timestep timestep = time - m_last_frame_time;
        m_last_frame_time = time;

        if (!m_minimized) {
            {
                HZ_PROFILE_SCOPE("LayerStack onUpdate");

                // 层级从左往右更新/渲染
                for (auto& layer : m_layer_stack) {
                    layer->onUpdate(timestep);
                }
            }

            // imgui 更新层
            m_imgui_layer->begin();
            {
                HZ_PROFILE_SCOPE("LayerStack onImGuiRender");

                for (auto& layer : m_layer_stack) {
                    layer->onImGuiRender();
                }
            }
            m_imgui_layer->end();
        }

        m_window->onUpdate();
    }
}

void Application::onEvent(Event& event)
{
    HZ_PROFILE_FUNCTION();

    EventDispatcher dispatcher{event};
    dispatcher.dispatch<WindowCloseEvent>(HZ_BIND_EVENT_FN(Application::onWindowClosed));
    dispatcher.dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(Application::onWindowResized));

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

bool Application::onWindowResized(WindowResizeEvent& e)
{
    HZ_PROFILE_FUNCTION();

    auto width = e.getResizeWidth();
    auto height = e.getResizeHeight();
    if (width == 0 || height == 0) {
        m_minimized = true;
        return false;
    }
    m_minimized = false;
    Renderer::onWindowResize(width, height);
    return false;
}

void Application::pushLayer(Layer* layer)
{
    HZ_PROFILE_FUNCTION();

    m_layer_stack.pushLayer(layer);
    layer->onAttach();
}

void Application::pushOverlay(Layer* overly)
{
    HZ_PROFILE_FUNCTION();

    m_layer_stack.pushOverlay(overly);
    overly->onAttach();
}

void Application::popLayer(Layer* layer)
{
    HZ_PROFILE_FUNCTION();

    m_layer_stack.popLayer(layer);
}

void Application::popOverlay(Layer* overly)
{
    HZ_PROFILE_FUNCTION();

    m_layer_stack.popOverlay(overly);
}
}  // namespace Hazel
