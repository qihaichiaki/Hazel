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
    m_temp_entity = m_active_scene->createEntity();
    // 添加简单sprit组件
    m_temp_entity.addComponent<SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});

    // 创建两个相机对象
    m_primary_camera_entity = m_active_scene->createEntity("主相机");
    m_second_camera_entity = m_active_scene->createEntity("第二个相机");
    // 给相机对象添加相机组件
    m_primary_camera_entity.addComponent<CameraComponent>();
    m_second_camera_entity.addComponent<CameraComponent>().Primary = false;
    // 给相机对象添加脚本组件
    // test
    class TestCameraMoveScript : public ScriptableEntity
    {
    public:
        void onCreate() override
        {
            HZ_WARN("你好, 世界!我是脚本桑, 现在挂载在对象:{}", getComponent<TagComponent>().Tag);
        }

        void onUpdate(Timestep ts) override
        {
            auto& transform_component = getComponent<TransformComponent>();
            const float speed = 10.0f;
            if (Input::isKeyPressed(KeyCode::W)) {
                transform_component.Translation.y += speed * ts;
            }
            if (Input::isKeyPressed(KeyCode::S)) {
                transform_component.Translation.y -= speed * ts;
            }
            if (Input::isKeyPressed(KeyCode::A)) {
                transform_component.Translation.x -= speed * ts;
            }
            if (Input::isKeyPressed(KeyCode::D)) {
                transform_component.Translation.x += speed * ts;
            }
        }
    };
    m_primary_camera_entity.addComponent<NativeScriptComponent>().bind<TestCameraMoveScript>();
    m_second_camera_entity.addComponent<NativeScriptComponent>().bind<TestCameraMoveScript>();

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
        // m_camera_controller.onResize(m_viewport_size.x, m_viewport_size.y);
    }

    if (m_is_viewport) {
        m_camera_controller.onUpdate(ts);
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
    m_camera_controller.onEvent(e);
}

static void createDockspace()
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
    ImGui::Separator();
    // ImGui::Text("场景信息:");
    // if (m_temp_entity) {
    //     ImGui::Text("%s", m_temp_entity.getComponent<TagComponent>().Tag.c_str());
    //     ImGui::ColorEdit4(
    //         "color",
    //         glm::value_ptr(m_temp_entity.getComponent<SpriteRendererComponent>().Color));
    // }
    // Entity m_renderer_camera =
    //     m_is_primary_camera ? m_primary_camera_entity : m_second_camera_entity;
    // if (m_renderer_camera) {
    //     ImGui::Text("当前渲染相机对象:%s",
    //                 m_renderer_camera.getComponent<TagComponent>().Tag.c_str());
    //     auto& render_transform = m_renderer_camera.getComponent<TransformComponent>().Transform;
    //     ImGui::Text("x: %.3f, y: %.3f", render_transform[3][0], render_transform[3][1]);
    // }
    // if (ImGui::Checkbox("激活主相机", &m_is_primary_camera)) {
    //     if (m_primary_camera_entity) {
    //         m_primary_camera_entity.getComponent<CameraComponent>().Primary =
    //         m_is_primary_camera;
    //     }
    //     if (m_second_camera_entity) {
    //         m_second_camera_entity.getComponent<CameraComponent>().Primary =
    //         !m_is_primary_camera;
    //     }
    // }
    // if (m_second_camera_entity) {
    //     auto& camera_component = m_second_camera_entity.getComponent<CameraComponent>();
    //     float view_size = camera_component.Camera.getOrthographicSize();
    //     ImGui::DragFloat("调整第二个相机的可视范围: ", &view_size, 1.0f, 0.001f, 100.0f);
    //     camera_component.Camera.setOrthographicSize(view_size);
    // }

    ImGui::Separator();
    ImGui::End();

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