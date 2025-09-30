#include "sandbox_2d.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D() : Layer{"Sandbox2D"}, m_camera_controller{1280.0f / 720.0f} {}

void Sandbox2D::onAttach()
{
    HZ_PROFILE_FUNCTION();

    m_texture = Hazel::Texture2D::create("assets/textures/Checkerboard.png");
    m_texture_2 = Hazel::Texture2D::create("assets/textures/namica.png");
}

void Sandbox2D::onDetach()
{
    HZ_PROFILE_FUNCTION();

    m_texture = nullptr;
}

void Sandbox2D::onUpdate(Hazel::Timestep ts)
{
    HZ_PROFILE_FUNCTION();

    // update
    m_camera_controller.onUpdate(ts);

    // render
    {
        HZ_PROFILE_SCOPE("Renderer Prep");
        Hazel::RendererCommand::setClearColor({0.2f, 0.2f, 0.2f, 1.0f});
        Hazel::RendererCommand::clear();
    }

    {
        HZ_PROFILE_SCOPE("Renderer Draw");
        Hazel::Renderer2D::beginScene(m_camera_controller.getCamera());
        Hazel::Renderer2D::drawQuad({0.0f, 0.0f, 0.1f}, {1.0f, 1.0f}, {0.8f, 0.4f, 0.2f, 1.0f});
        Hazel::Renderer2D::drawQuad({1.23f, 0.14f, 0.0f}, {1.0f, 2.0f}, {0.2f, 0.8f, 0.2f, 1.0f});
        Hazel::Renderer2D::drawQuad({0.0f, 0.0f, -0.9f}, {10.0f, 10.0f}, m_texture);
        Hazel::Renderer2D::drawQuad({2.0f, 3.0f, -0.8f}, {5.0f, 5.0f}, m_texture_2);
        Hazel::Renderer2D::endScene();
    }
}

void Sandbox2D::onEvent(Hazel::Event& e)
{
    m_camera_controller.onEvent(e);
}

void Sandbox2D::onImGuiRender()
{
    HZ_PROFILE_FUNCTION();

    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_square_color));
    ImGui::End();
}