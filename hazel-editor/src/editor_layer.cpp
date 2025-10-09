#include "editor_layer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{

EditorLayer::EditorLayer() : Layer{"EditorLayer"}, m_camera_controller{2560.0f / 1600.0f} {}

void EditorLayer::onAttach()
{
    // 创建帧缓冲区
    m_framebuffer = Framebuffer::create(FramebufferSpecification{1536, 960});

    // 创建初始场景
    m_active_scene = createRef<Scene>();

    // 创建entity
    m_temp_entity = m_active_scene->createEntity("示例对象");
    // 添加简单sprit组件
    m_temp_entity.addComponent<SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
}

void EditorLayer::onDetach()
{
    m_active_scene = nullptr;
}

void EditorLayer::onUpdate(Hazel::Timestep ts)
{
    // update
    if (m_is_viewport) {
        m_camera_controller.onUpdate(ts);
    }

    Hazel::Renderer2D::resetStats();
    // render
    m_framebuffer->bind();
    Hazel::RendererCommand::setClearColor({0.2f, 0.2f, 0.2f, 1.0f});
    Hazel::RendererCommand::clear();

    Hazel::Renderer2D::beginScene(m_camera_controller.getCamera());
    m_active_scene->onUpdate(ts);
    Hazel::Renderer2D::endScene();
    m_framebuffer->unBind();
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
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", renderer2d_stats.DrawCalls);
    ImGui::Text("Quad Count: %d", renderer2d_stats.QuadCount);
    ImGui::Text("Vertex Count: %d", renderer2d_stats.getTotalVertexCount());
    ImGui::Text("Index Count: %d", renderer2d_stats.getTotalIndexCount());
    ImGui::Separator();
    if (m_temp_entity) {
        ImGui::Text("%s", m_temp_entity.getComponent<TagComponent>().Tag.c_str());
        ImGui::ColorEdit4(
            "color", glm::value_ptr(m_temp_entity.getComponent<SpriteRendererComponent>().Color));
    }
    ImGui::Separator();
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
    ImGui::Begin("视口");
    // 检查当前窗口是否是聚焦和悬浮的
    m_is_viewport = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
    // 如果均不是, 则阻止imgui层事件的传播
    Application::get().getImGuiLayer()->setBlockEvents(!m_is_viewport);

    ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
    // 内存布局一致, 可以强转
    if (m_viewport_size != *(glm::vec2*)(&viewport_panel_size) && viewport_panel_size.x > 0 &&
        viewport_panel_size.y > 0) {
        m_viewport_size.x = viewport_panel_size.x;
        m_viewport_size.y = viewport_panel_size.y;
        // 1. 帧缓冲区改变大小
        m_framebuffer->resize((uint32_t)m_viewport_size.x, (uint32_t)m_viewport_size.y);
        // 2. 重新设置相机投影矩阵
        m_camera_controller.onResize(m_viewport_size.x, m_viewport_size.y);
    }
    ImGui::Image((void*)(uintptr_t)(m_framebuffer->getColorAttachmentRendererID()),
                 ImVec2{m_viewport_size.x, m_viewport_size.y}, ImVec2{0, 1},
                 ImVec2{1, 0});  // v方向反转一下
    ImGui::End();
    ImGui::PopStyleVar();
}

}  // namespace Hazel