#include "editor_layer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <Hazel/Scene/scene_serializer.h>
#include <Hazel/Utils/platform_utils.h>
#include <Hazel/Math/math.h>
#include <ImGuizmo.h>

namespace Hazel
{

EditorLayer::EditorLayer() : Layer{"EditorLayer"} {}

void EditorLayer::onAttach()
{
    // 创建帧缓冲区
    FramebufferSpecification framebuffer_spec;
    framebuffer_spec.setSize(1536, 960);
    framebuffer_spec.setAttachmentSpecification({FramebufferTextureFormat::RGBA8,
                                                 FramebufferTextureFormat::RED_INTEGER,
                                                 FramebufferTextureFormat::DEPTH24_STENCIL8});
    m_framebuffer = Framebuffer::create(framebuffer_spec);

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
        (m_viewport_size.x != spec.getWitdh() || m_viewport_size.y != spec.getHeight())) {
        // 1. 帧缓冲区改变大小
        m_framebuffer->resize((uint32_t)m_viewport_size.x, (uint32_t)m_viewport_size.y);
        // 2. 重新设置场景视口大小
        m_active_scene->onViewportResize((uint32_t)m_viewport_size.x, (uint32_t)m_viewport_size.y);
        // 3. 设置编辑器相机的视口大小
        m_editor_camera.setViewportSize(m_viewport_size.x, m_viewport_size.y);
    }

    // 编辑器相机更新
    m_editor_camera.onUpdate(ts);
    Hazel::Renderer2D::resetStats();

    // render
    m_framebuffer->bind();
    Hazel::RendererCommand::setClearColor({0.2f, 0.2f, 0.2f, 1.0f});
    Hazel::RendererCommand::clear();

    // 帧缓冲区的第二个实体id附加区域清空
    m_framebuffer->clearAttachment(1, -1);

    // m_active_scene->onUpdateRuntime(ts);
    m_active_scene->onUpdateEditor(ts, m_editor_camera);

    // 获取当前鼠标在视口中的坐标
    auto [m_x, m_y] = ImGui::GetMousePos();
    m_x -= m_viewport_bounds[0].x;
    m_y -= m_viewport_bounds[0].y;
    // 需要反转y, 因为当前opengl渲染纹理是左下角0, 0并非左上角
    auto viewport_size = m_viewport_bounds[1] - m_viewport_bounds[0];
    m_y = viewport_size.y - m_y;

    if (m_x >= 0 && m_y >= 0 && m_x <= viewport_size.x && m_y <= viewport_size.y) {
        int pixel_id = m_framebuffer->readPixel(1, (int)m_x, (int)m_y);
        m_hover_entity =
            pixel_id == -1 ? Entity{} : Entity{(entt::entity)pixel_id, m_active_scene.get()};
    }

    m_framebuffer->unBind();
}

void EditorLayer::onEvent(Hazel::Event& e)
{
    m_editor_camera.onEvent(e);

    EventDispatcher dispatcher{e};
    dispatcher.dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::onKeyPressed));
    dispatcher.dispatch<MouseButtonPressedEvent>(
        HZ_BIND_EVENT_FN(EditorLayer::onMouseButtonPressed));
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

        case KeyCode::Q:
            m_gizom_type = -1;
            break;
        case KeyCode::W:
            m_gizom_type = ImGuizmo::OPERATION::TRANSLATE;
            break;
        case KeyCode::E:
            m_gizom_type = ImGuizmo::OPERATION::ROTATE;
            break;
        case KeyCode::R:
            m_gizom_type = ImGuizmo::OPERATION::SCALE;
            break;
        default:
            break;
    }

    return false;
}

bool EditorLayer::onMouseButtonPressed(MouseButtonPressedEvent& event)
{
    if (event.getMouseButton() == (int)KeyCode::MouseButtonLeft) {
        if (!ImGuizmo::IsOver() && !Input::isKeyPressed(KeyCode::LeftAlt)) {
            m_scene_hierarchy_panel.setSelectedEntity(m_hover_entity);
            return true;
        }
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
    style.WindowMinSize.x = 380.0f;  // 窗口被停靠, 宽度至少为370
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
        if (ImGui::BeginMenu("变换")) {
            if (ImGui::MenuItem("关闭", "Q")) {
                m_gizom_type = -1;
            }
            if (ImGui::MenuItem("平移", "W")) {
                m_gizom_type = ImGuizmo::OPERATION::TRANSLATE;
            }
            if (ImGui::MenuItem("旋转", "E")) {
                m_gizom_type = ImGuizmo::OPERATION::ROTATE;
            }
            if (ImGui::MenuItem("缩放", "R")) {
                m_gizom_type = ImGuizmo::OPERATION::SCALE;
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
    m_content_browser_panel.onImGuiRenderer();

    auto renderer2d_stats = Hazel::Renderer2D::getStats();
    ImGui::Begin("面板信息");
    char const* hover_entity_tag = "无";
    if (m_hover_entity) {
        hover_entity_tag = m_hover_entity.getComponent<TagComponent>().Tag.c_str();
    }
    ImGui::Text("当前悬停的实体对象: %s", hover_entity_tag);
    ImGui::Separator();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", renderer2d_stats.DrawCalls);
    ImGui::Text("Quad Count: %d", renderer2d_stats.QuadCount);
    ImGui::Text("Vertex Count: %d", renderer2d_stats.getTotalVertexCount());
    ImGui::Text("Index Count: %d", renderer2d_stats.getTotalIndexCount());
    ImGui::End();

    // viewport
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
    ImGui::Begin("视口");
    auto viewport_min_region = ImGui::GetWindowContentRegionMin();
    auto viewport_max_region = ImGui::GetWindowContentRegionMax();
    auto viewport_offset = ImGui::GetWindowPos();
    m_viewport_bounds[0] = {viewport_min_region.x + viewport_offset.x,
                            viewport_min_region.y + viewport_offset.y};
    m_viewport_bounds[1] = {viewport_max_region.x + viewport_offset.x,
                            viewport_max_region.y + viewport_offset.y};

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

    // 显示变换组件 - Gizmos
    Entity selected_entity = m_scene_hierarchy_panel.getSelectedEntity();
    if (m_gizom_type != -1 && selected_entity) {
        ImGuizmo::SetOrthographic(false);  // 使其适用于透视投影
        ImGuizmo::SetDrawlist();

        // 相机的视图矩阵
        const auto& camera_view = m_editor_camera.getViewMatrix();
        // 相机的投影矩阵
        const auto& camera_projection = m_editor_camera.getProjection();

        // entity的transform
        auto& entity_transform_component = selected_entity.getComponent<TransformComponent>();
        auto transform = entity_transform_component.getTransform();

        ImGuizmo::SetRect(m_viewport_bounds[0].x, m_viewport_bounds[0].y, m_viewport_size.x,
                          m_viewport_size.y);

        // Snapping 精确控制
        bool snap = Input::isKeyPressed(KeyCode::LeftControl);
        float snap_value = 0.5f;
        if (m_gizom_type == ImGuizmo::OPERATION::ROTATE) {
            snap_value = 45.0f;
        }

        float snap_values[3] = {snap_value, snap_value, snap_value};

        ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
                             (ImGuizmo::OPERATION)m_gizom_type, ImGuizmo::MODE::LOCAL,
                             glm::value_ptr(transform), nullptr, snap ? snap_values : nullptr);
        // 如果发生平移了
        if (ImGuizmo::IsUsing()) {
            // 需要分解函数, 讲改变之后的entity_transform进行分解为平移，旋转，缩放
            // glm::decompose() 类似的功能, 但是其中处理了很多当前不关心的
            // 所以改为Hazel自己实现的版本
            glm::vec3 translation, rotation, scale;
            Hazel::Math::decomposeTransform(transform, translation, rotation, scale);

            entity_transform_component.Translation = translation;
            entity_transform_component.Rotation = rotation;
            entity_transform_component.Scale = scale;
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();
}

}  // namespace Hazel