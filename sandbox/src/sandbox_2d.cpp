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

    Hazel::Renderer2D::resetStats();

    static float rotation = 0.0f;
    float rotation_speed = 1.5f;
    rotation += rotation_speed * ts;
    if (rotation >= 2.0f * 3.1415926535f) {
        rotation = 0.0f;
    }

    {
        HZ_PROFILE_SCOPE("Renderer Draw");
        Hazel::Renderer2D::beginScene(m_camera_controller.getCamera());
        Hazel::Renderer2D::drawQuad({0.0f, 0.0f, 0.1f}, {1.0f, 1.0f}, {0.8f, 0.4f, 0.2f, 1.0f});
        // 旋转矩形绘制
        Hazel::Renderer2D::drawRotatedQuad({0.0f, 0.0f, 0.2f}, {0.5f, 0.5f}, rotation,
                                           m_square_color);
        Hazel::Renderer2D::drawQuad({1.0f, 0.14f, 0.0f}, {1.0f, 2.0f}, {0.2f, 0.8f, 0.2f, 1.0f});
        // 绘制马赛克背景
        Hazel::Renderer2D::drawQuad({0.0f, 0.0f, -0.9f}, {20.0f, 20.0f}, m_texture, glm::vec4{1.0f},
                                    10.0f);
        // 绘制nanamichiaki
        Hazel::Renderer2D::drawQuad({2.0f, 3.0f, 0.5f}, {2.0f, 2.0f}, m_texture_2,
                                    glm::vec4{0.8f, 1.0f, 0.9f, 1.0f});

        // 压力测试 100 * 100 -> 10000
        for (float y = -5.0f; y < 5.0f; y += 0.25f) {
            for (float x = -5.0f; x < 5.0f; x += 0.25f) {
                glm::vec4 color = {(x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f};
                Hazel::Renderer2D::drawQuad({x, y, -0.5}, {0.45f, 0.45f}, color);
            }
        }
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

    auto renderer2d_stats = Hazel::Renderer2D::getStats();

    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_square_color));
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", renderer2d_stats.DrawCalls);
    ImGui::Text("Quad Count: %d", renderer2d_stats.QuadCount);
    ImGui::Text("Vertex Count: %d", renderer2d_stats.getTotalVertexCount());
    ImGui::Text("Index Count: %d", renderer2d_stats.getTotalIndexCount());
    ImGui::End();
}