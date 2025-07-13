#include "sandbox_2d.h"

#include <PlatForm/OpenGL/opengl_shader.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D() : Layer{"Sandbox2D"}, m_camera_controller{1280.0f / 720.0f} {}

void Sandbox2D::onAttach()
{
    m_square_va = Hazel::VertexArray::create();

    // clang-format off
    float square_vertices[3 * 4] = {
        -0.75f, -0.75f, 0.0f,
        0.75f, -0.75f, 0.0f,
        0.75f, 0.75f, 0.0f, 
        -0.75f, 0.75f, 0.0f
    };
    // clang-format on
    std::shared_ptr<Hazel::VertexBuffer> square_vertex_buffer;
    square_vertex_buffer = Hazel::VertexBuffer::create(square_vertices, sizeof(square_vertices));

    square_vertex_buffer->setLayout({{Hazel::ShaderDataType::Float3, "a_Position"}});
    m_square_va->addVertexBuffer(square_vertex_buffer);

    uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};  // 逆时针 绘制顺序
    std::shared_ptr<Hazel::IndexBuffer> square_index_buffer;
    square_index_buffer =
        Hazel::IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t));
    m_square_va->setIndexBuffer(square_index_buffer);

    m_square_shader = Hazel::Shader::create("sandbox/assets/shaders/square.glsl");
}

void Sandbox2D::onDetach() {}

void Sandbox2D::onUpdate(Hazel::Timestep ts)
{
    m_camera_controller.onUpdate(ts);
    Hazel::RendererCommand::setClearColor({0.2f, 0.2f, 0.2f, 1.0f});
    Hazel::RendererCommand::clear();

    Hazel::Renderer::beginScene(m_camera_controller.getCamera());
    m_square_shader->bind();
    std::static_pointer_cast<Hazel::OpenGLShader>(m_square_shader)
        ->uploadUniformFloat4("u_Color", m_square_color);

    Hazel::Renderer::submit(m_square_shader, m_square_va);

    Hazel::Renderer::endScene();
}

void Sandbox2D::onEvent(Hazel::Event& e)
{
    m_camera_controller.onEvent(e);
}

void Sandbox2D::onImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_square_color));
    ImGui::End();
}