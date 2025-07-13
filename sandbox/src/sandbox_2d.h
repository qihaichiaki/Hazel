#include <hazel.h>

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
    Hazel::Ref<Hazel::Shader> m_square_shader;
    Hazel::Ref<Hazel::VertexArray> m_square_va;
    Hazel::OrthoGraphicCameraController m_camera_controller;

    glm::vec4 m_square_color{1.0f};
};