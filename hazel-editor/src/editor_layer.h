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

    Ref<Scene> m_active_scene = nullptr;  // 示例激活scene
    Entity m_temp_entity;                 // 示例实体对象
    Entity m_primary_camera_entity;       // 主要相机对象
    Entity m_second_camera_entity;        // 第二个相机对象

    bool m_is_primary_camera = true;  // 主相机是否时当前的渲染相机
    bool m_is_viewport = false;       // 是否激活在视口上的事件更新
    glm::vec4 m_square_color{1.0f};
    glm::vec2 m_viewport_size = {0.0f, 0.0f};
};
}  // namespace Hazel
