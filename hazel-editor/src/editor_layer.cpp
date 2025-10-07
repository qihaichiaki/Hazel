#include "editor_layer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{

EditorLayer::EditorLayer() : Layer{"EditorLayer"}, m_camera_controller{2560.0f / 1600.0f} {}

void EditorLayer::onAttach()
{
    HZ_PROFILE_FUNCTION();

    m_texture = Texture2D::create("assets/textures/Checkerboard.png");
    m_texture_2 = Texture2D::create("assets/textures/namica.png");
    m_framebuffer = Framebuffer::create(FramebufferSpecification{1536, 960});
}

void EditorLayer::onDetach()
{
    HZ_PROFILE_FUNCTION();

    m_texture = nullptr;
}

void EditorLayer::onUpdate(Hazel::Timestep ts)
{
    HZ_PROFILE_FUNCTION();

    // update
    m_camera_controller.onUpdate(ts);

    Hazel::Renderer2D::resetStats();

    // render
    {
        HZ_PROFILE_SCOPE("Renderer Prep");
        m_framebuffer->bind();
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
        m_framebuffer->unBind();
    }
}

void EditorLayer::onEvent(Hazel::Event& e)
{
    m_camera_controller.onEvent(e);
}

void EditorLayer::onImGuiRender()
{
    HZ_PROFILE_FUNCTION();

    // dockspace 创建停靠区域
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static bool dockspace_open = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    } else {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking,
    // otherwise any change of dockspace/settings would lead to windows being stuck in limbo and
    // never being visible.
    if (!opt_padding) ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspace_open, window_flags);
    if (!opt_padding) ImGui::PopStyleVar();

    if (opt_fullscreen) ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("文件")) {
            if (ImGui::MenuItem("关闭")) {
                Application::get().close();
            }
            ImGui::Separator();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();

    auto renderer2d_stats = Hazel::Renderer2D::getStats();
    ImGui::Begin("渲染信息");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_square_color));
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", renderer2d_stats.DrawCalls);
    ImGui::Text("Quad Count: %d", renderer2d_stats.QuadCount);
    ImGui::Text("Vertex Count: %d", renderer2d_stats.getTotalVertexCount());
    ImGui::Text("Index Count: %d", renderer2d_stats.getTotalIndexCount());
    ImGui::End();

    ImGui::Begin("场景窗口");
    ImGui::Image((void*)(uintptr_t)(m_framebuffer->getColorAttachmentRendererID()),
                 ImVec2{1536, 960}, ImVec2{0, 1}, ImVec2{1, 0});  // v方向反转一下
    ImGui::End();
}

}  // namespace Hazel