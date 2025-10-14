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

private:
    OrthoGraphicCameraController m_camera_controller;
    Ref<Framebuffer> m_framebuffer;

    SceneHierarchyPanel m_scene_hierarchy_panel;
    Ref<Scene> m_active_scene = nullptr;  // 示例激活scene

    bool m_is_viewport = false;  // 是否激活在视口上的事件更新
    glm::vec2 m_viewport_size = {0.0f, 0.0f};
};
}  // namespace Hazel
