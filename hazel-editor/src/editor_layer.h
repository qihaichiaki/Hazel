#pragma once

#include <hazel.h>
#include "Panels/scene_hierarchy_panel.h"
#include "Panels/content_browser_panel.h"

namespace Hazel
{
class EditorLayer : public Layer
{
public:
    EditorLayer();
    ~EditorLayer() = default;

    void onAttach() override;
    void onDetach() override;
    void onUpdate(Hazel::Timestep) override;
    void onEvent(Hazel::Event&) override;
    void onImGuiRender() override;

private:
    void uiPlayBoard();

private:
    void createDockspace();
    void newScene();
    void openScene();
    void openScene(const std::filesystem::path& scene_path);
    void saveSceneAs();
    void saveScene();
    void duplicateEntity();

    bool onKeyPressed(KeyPressedEvent& event);
    bool onMouseButtonPressed(MouseButtonPressedEvent& event);

private:
    enum class SceneState { Editor, Runtime };

    SceneState m_scene_state{SceneState::Editor};

private:
    Ref<Framebuffer> m_framebuffer;
    std::vector<Ref<Texture2D>> m_editor_icons;

    SceneHierarchyPanel m_scene_hierarchy_panel;
    ContentBrowserPanel m_content_browser_panel;

    Ref<Scene> m_active_scene{nullptr};  // 当前激活scene
    Ref<Scene> m_eidtor_scene{nullptr};  // 编辑器场景
    std::string m_current_scene_path{};

    EditorCamera m_editor_camera;

    bool m_is_viewport{false};  // 是否激活在视口上的事件更新
    glm::vec2 m_viewport_size{0.0f, 0.0f};
    glm::vec2 m_viewport_bounds[2];

    int m_gizom_type{-1};     // 变换组件类型, 默认是关闭
    Entity m_hover_entity{};  // 当前悬浮在哪个entity的上面
};
}  // namespace Hazel
