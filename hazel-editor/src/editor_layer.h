#pragma once

#include <hazel.h>
#include "Panels/scene_hierarchy_panel.h"

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
    void createDockspace();
    void newScene();
    void openScene();
    void saveSceneAs();

    bool onKeyPressed(KeyPressedEvent& event);

private:
    Ref<Framebuffer> m_framebuffer;

    SceneHierarchyPanel m_scene_hierarchy_panel;
    Ref<Scene> m_active_scene = nullptr;  // 示例激活scene

    EditorCamera m_editor_camera;

    bool m_is_viewport = false;  // 是否激活在视口上的事件更新
    glm::vec2 m_viewport_size = {0.0f, 0.0f};

    int m_gizom_type{-1};  // 变换组件类型, 默认是关闭
};
}  // namespace Hazel
