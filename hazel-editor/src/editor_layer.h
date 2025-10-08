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
    Ref<Texture2D> m_texture;
    Ref<Texture2D> m_texture_2;
    Ref<Framebuffer> m_framebuffer;

    bool m_is_viewport = false;  // 是否激活在视口上的事件更新
    glm::vec4 m_square_color{1.0f};
    glm::vec2 m_viewport_size = {0.0f, 0.0f};
};
}  // namespace Hazel
