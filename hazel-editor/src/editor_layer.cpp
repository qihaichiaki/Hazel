#include "editor_layer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <Hazel/Scene/scene_serializer.h>
#include <Hazel/Utils/platform_utils.h>
#include <ImGuizmo.h>

namespace Hazel
{

EditorLayer::EditorLayer() : Layer{"EditorLayer"} {}

void EditorLayer::onAttach()
{
    // 创建帧缓冲区
    m_framebuffer = Framebuffer::create(FramebufferSpecification{1536, 960});

    // 创建初始场景
    m_active_scene = createRef<Scene>();

    // 场景视口添加当前激活场景
    m_scene_hierarchy_panel.setContext(m_active_scene);
}

void EditorLayer::onDetach()
{
    m_active_scene = nullptr;
}

void EditorLayer::onUpdate(Hazel::Timestep ts)
{
    // update
    // 判断当前view 视图是否更新宽度和高度, 方便进行设置场景的视图宽高
    if (const auto& spec = m_framebuffer->getSpecification();
        m_viewport_size.x > 0.0f && m_viewport_size.y > 0.0f &&
        (m_viewport_size.x != spec.Width || m_viewport_size.y != spec.Height)) {
        // 1. 帧缓冲区改变大小
        m_framebuffer->resize((uint32_t)m_viewport_size.x, (uint32_t)m_viewport_size.y);
        // 2. 重新设置相机投影矩阵
        m_active_scene->onViewportResize((uint32_t)m_viewport_size.x, (uint32_t)m_viewport_size.y);
    }

    Hazel::Renderer2D::resetStats();
    // render
    m_framebuffer->bind();
    Hazel::RendererCommand::setClearColor({0.2f, 0.2f, 0.2f, 1.0f});
    Hazel::RendererCommand::clear();

    // Hazel::Renderer2D::beginScene(m_camera_controller.getCamera());
    m_active_scene->onUpdate(ts);
    // Hazel::Renderer2D::endScene();
    m_framebuffer->unBind();
}

void EditorLayer::onEvent(Hazel::Event& e)
{
    EventDispatcher dispatcher{e};
    dispatcher.dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::onKeyPressed));
}

bool EditorLayer::onKeyPressed(KeyPressedEvent& event)
{
    // 短按, 长按排除
    if (event.getRepeatCount() > 0) {
        return false;
    }

    bool is_ctrl =
        Input::isKeyPressed(KeyCode::LeftControl) || Input::isKeyPressed(KeyCode::RightControl);
    bool is_shift =
        Input::isKeyPressed(KeyCode::LeftShift) || Input::isKeyPressed(KeyCode::RightShift);

    switch ((KeyCode)event.getKeyCode()) {
        case KeyCode::N:
            if (is_ctrl) {
                newScene();
            }
            break;
        case KeyCode::O:
            if (is_ctrl) {
                openScene();
            }
            break;
        case KeyCode::S:
            if (is_ctrl && is_shift) {
                saveSceneAs();
            }
            break;
        default:
            break;
    }

    return false;
}

void EditorLayer::newScene()
{
    m_active_scene = createRef<Scene>();  // 创建一个新的场景
    m_active_scene->onViewportResize((uint32_t)m_viewport_size.x, (uint32_t)m_viewport_size.y);
    m_scene_hierarchy_panel.setContext(m_active_scene);
}

void EditorLayer::openScene()
{
    auto file_path = FileDialogs::openFile("Hazel Scene (*.hazel)\0*.hazel\0");
    if (!file_path.empty()) {
        m_active_scene = createRef<Scene>();  // 创建一个新的场景
        m_active_scene->onViewportResize((uint32_t)m_viewport_size.x, (uint32_t)m_viewport_size.y);
        m_scene_hierarchy_panel.setContext(m_active_scene);
        SceneSerializer serializer{m_active_scene};
        serializer.deserialize(file_path);
    }
}

void EditorLayer::saveSceneAs()
{
    auto file_path = FileDialogs::saveFile("Hazel Scene (*.hazel)\0*.hazel\0");
    if (!file_path.empty()) {
        SceneSerializer serializer{m_active_scene};
        serializer.serialize(file_path);
    }
}

void EditorLayer::createDockspace()
{
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
    ImGuiStyle& style = ImGui::GetStyle();
    float min_win_size_x = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;  // 窗口被停靠, 宽度至少为370
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    style.WindowMinSize.x = min_win_size_x;  // 还原

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("开始")) {
            if (ImGui::MenuItem("新建场景", "CTRL+N")) {
                newScene();
            }
            if (ImGui::MenuItem("打开场景...", "CTRL+O")) {
                openScene();
            }
            if (ImGui::MenuItem("另存为场景...", "CTRL+SHIFT+S")) {
                saveSceneAs();
            }
            if (ImGui::MenuItem("关闭")) {
                Application::get().close();
            }
            ImGui::Separator();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void EditorLayer::onImGuiRender()
{
    HZ_PROFILE_FUNCTION();

    createDockspace();

    // 渲染场景层次面板
    m_scene_hierarchy_panel.onImGuiRenderer();

    auto renderer2d_stats = Hazel::Renderer2D::getStats();
    ImGui::Begin("渲染信息");
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", renderer2d_stats.DrawCalls);
    ImGui::Text("Quad Count: %d", renderer2d_stats.QuadCount);
    ImGui::Text("Vertex Count: %d", renderer2d_stats.getTotalVertexCount());
    ImGui::Text("Index Count: %d", renderer2d_stats.getTotalIndexCount());
    ImGui::End();

    // 显示变换组件
    Entity select_entity = m_scene_hierarchy_panel.getSelectionEntity();
    if (select_entity) {
        // TODO: 临时获取当前场景渲染的相机对象
        auto camera_entity = m_active_scene->getPrimaryCameraEntity();
        if (camera_entity) {
            // 转换小组件调用
            ImGuizmo::BeginFrame();

            auto& camera = camera_entity.getComponent<CameraComponent>().Camera;
            // 相机的视图矩阵
            auto& transform_component = camera_entity.getComponent<TransformComponent>();
            auto view_transform = transform_component.getTransform();  // 逆置一下
            // 相机的投影矩阵
            auto projection = camera.getProjection();

            ImGuizmo::SetRect(0, 0, m_viewport_size.x, m_viewport_size.y);
            ImGuizmo::Manipulate(glm::value_ptr(view_transform), glm::value_ptr(projection),
                                 ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL,
                                 glm::value_ptr(view_transform));
            // 如果发生平移了
            if (ImGuizmo::IsUsing()) {
                // 测试transf
                transform_component.Translation = glm::vec3{view_transform[3]};
            }
        }
    }

    // viewport
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
    ImGui::Begin("视口");
    // 检查当前窗口是否是聚焦和悬浮的
    m_is_viewport = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
    // 如果均不是, 则阻止imgui层事件的传播
    Application::get().getImGuiLayer()->setBlockEvents(!m_is_viewport);

    ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
    m_viewport_size = {viewport_panel_size.x, viewport_panel_size.y};

    // 显示视图. v方向上反转一下
    ImGui::Image((void*)(uintptr_t)(m_framebuffer->getColorAttachmentRendererID()),
                 ImVec2{m_viewport_size.x, m_viewport_size.y}, ImVec2{0, 1},
                 ImVec2{1, 0});  // v方向反转一下
    ImGui::End();
    ImGui::PopStyleVar();
}

}  // namespace Hazel