#pragma once

#include <hazel.h>

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
    OrthoGraphicCameraController m_camera_controller;
    Ref<Framebuffer> m_framebuffer;

    Entity m_temp_entity;                 // 示例实体对象
    Ref<Scene> m_active_scene = nullptr;  // 示例激活scene

    bool m_is_viewport = false;  // 是否激活在视口上的事件更新
    glm::vec4 m_square_color{1.0f};
    glm::vec2 m_viewport_size = {0.0f, 0.0f};
};
}  // namespace Hazel
