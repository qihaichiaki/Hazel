#include <hazel.h>

#include "particle_system.h"

class Sandbox2D : public Hazel::Layer
{
public:
    Sandbox2D();
    ~Sandbox2D() = default;

    void onAttach() override;
    void onDetach() override;
    void onUpdate(Hazel::Timestep) override;
    void onEvent(Hazel::Event&) override;
    void onImGuiRender() override;

private:
    Hazel::OrthoGraphicCameraController m_camera_controller;
    Hazel::Ref<Hazel::Texture2D> m_texture;
    Hazel::Ref<Hazel::Texture2D> m_texture_2;

    glm::vec4 m_square_color{1.0f};
    ParticleSystem particle_system;
    ParticleProps particle_props;
};