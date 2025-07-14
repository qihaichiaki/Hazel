#include "sandbox_2d.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D() : Layer{"Sandbox2D"}, m_camera_controller{1280.0f / 720.0f} {}

void Sandbox2D::onAttach() {}

void Sandbox2D::onDetach() {}

void Sandbox2D::onUpdate(Hazel::Timestep ts)
{
    m_camera_controller.onUpdate(ts);
    Hazel::RendererCommand::setClearColor({0.2f, 0.2f, 0.2f, 1.0f});
    Hazel::RendererCommand::clear();

    Hazel::Renderer2D::beginScene(m_camera_controller.getCamera());
    // TODO: z轴存在问题
    Hazel::Renderer2D::drawQuad({0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0.8f, 0.4f, 0.2f, 1.0f});
    Hazel::Renderer2D::drawQuad({0.23f, 0.14f}, {1.0f, 2.0f}, {0.2f, 0.8f, 0.2f, 1.0f});

    Hazel::Renderer2D::endScene();
}

void Sandbox2D::onEvent(Hazel::Event& e)
{
    m_camera_controller.onEvent(e);
}

void Sandbox2D::onImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_square_color));
    ImGui::End();
}