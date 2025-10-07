#include "sandbox_2d.h"
#include "random.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D() : Layer{"Sandbox2D"}, m_camera_controller{2560.0f / 1600.0f} {}

void Sandbox2D::onAttach()
{
    HZ_PROFILE_FUNCTION();

    m_texture = Hazel::Texture2D::create("assets/textures/Checkerboard.png");
    m_texture_2 = Hazel::Texture2D::create("assets/textures/namica.png");

    Random::init();
    m_particle_props.ColorBegin = {254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f};
    m_particle_props.ColorEnd = {254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f};
    m_particle_props.SizeBegin = 0.3f;
    m_particle_props.SizeVariation = 0.3f;
    m_particle_props.SizeEnd = 0.0f;
    m_particle_props.LifeTime = 5.0f;
    m_particle_props.Velocity = {0.0f, 0.0f};
    m_particle_props.VelocityVariation = {3.0f, 1.0f};
    m_particle_props.Position = {10.0f, 0.0f};
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

    Hazel::Renderer2D::resetStats();

    // render
    {
        HZ_PROFILE_SCOPE("Renderer Prep");
        Hazel::RendererCommand::setClearColor({0.2f, 0.2f, 0.2f, 1.0f});
        Hazel::RendererCommand::clear();
    }

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

    {
        if (Hazel::Input::isMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT)) {
            auto [pos_x, pos_y] = Hazel::Input::getMousePosition();  // 获取鼠标的坐标
            auto& window = Hazel::Application::get().getWindow();
            const auto& bounds = m_camera_controller.getBounds();
            auto camera_pos = m_camera_controller.getCamera().getPosition();

            float dx = (pos_x / window.getWidth()) * bounds.getWidth() - bounds.getWidth() * 0.5f;
            float dy =
                bounds.getHeight() * 0.5f - (pos_y / window.getHeight()) * bounds.getHeight();

            m_particle_props.Position = {camera_pos.x + dx, camera_pos.y + dy};
            for (int i = 0; i < m_particle_count; i++) {
                m_particle_system.emit(m_particle_props);
            }
        }
        m_particle_system.onUpdate(ts);
        m_particle_system.onRender(m_camera_controller.getCamera());
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

    ImGui::Begin("渲染信息");
    // ImGui::ColorEdit4("Square Color", glm::value_ptr(m_square_color));
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", renderer2d_stats.DrawCalls);
    ImGui::Text("Quad Count: %d", renderer2d_stats.QuadCount);
    ImGui::Text("Vertex Count: %d", renderer2d_stats.getTotalVertexCount());
    ImGui::Text("Index Count: %d", renderer2d_stats.getTotalIndexCount());
    ImGui::End();

    ImGui::Begin("粒子设置");
    ImGui::ColorEdit4("起始颜色", glm::value_ptr(m_particle_props.ColorBegin));
    ImGui::ColorEdit4("终止颜色", glm::value_ptr(m_particle_props.ColorEnd));
    ImGui::SliderFloat("起始大小", &m_particle_props.SizeBegin, 0.1f, 1.0f);
    ImGui::SliderFloat("起始大小变化量", &m_particle_props.SizeVariation, 0.3f, 0.5f);
    ImGui::SliderFloat("终止大小", &m_particle_props.SizeEnd, 0.0f, 0.3f);
    ImGui::DragFloat("生存时间", &m_particle_props.LifeTime, 1.0f, 0.0f, 1000.0f);
    ImGui::DragFloat2("起始速度", glm::value_ptr(m_particle_props.Velocity), 1.0f, 0.0f, 10.0f);
    ImGui::DragFloat2("速度变化值", glm::value_ptr(m_particle_props.VelocityVariation), 1.0f, 0.0f,
                      10.0f);
    ImGui::DragInt("单帧粒子发射数量:", &m_particle_count, 1.0, 0, 100);
    ImGui::End();
}