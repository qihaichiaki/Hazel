#include <hazel.h>
#include <Hazel/Core/entry_point.h>
#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <PlatForm/OpenGL/opengl_shader.h>
#include <glm/gtc/type_ptr.hpp>

#include "sandbox_2d.h"

class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() : Layer("Example"), m_camera_controller{1280.0f / 720.0f}
    {
        // triangle render
        m_triangle_va = Hazel::VertexArray::create();

        // clang-format off
        float triangle_vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
            0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
            0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };
        // clang-format on
        std::shared_ptr<Hazel::VertexBuffer> triangle_vertex_buffer;
        triangle_vertex_buffer =
            Hazel::VertexBuffer::create(triangle_vertices, sizeof(triangle_vertices));

        Hazel::BufferLayer triangle_vertex_layer{{Hazel::ShaderDataType::Float3, "a_Position"},
                                                 {Hazel::ShaderDataType::Float4, "a_Color"}};
        triangle_vertex_buffer->setLayout(triangle_vertex_layer);
        m_triangle_va->addVertexBuffer(triangle_vertex_buffer);

        uint32_t triangle_indices[3] = {0, 1, 2};  // 逆时针 绘制顺序
        std::shared_ptr<Hazel::IndexBuffer> triangle_index_buffer;
        triangle_index_buffer = Hazel::IndexBuffer::create(
            triangle_indices, sizeof(triangle_indices) / sizeof(uint32_t));
        m_triangle_va->setIndexBuffer(triangle_index_buffer);

        // shader对象
        // vertex shader
        std::string triangle_vertex_shader = R"(
            #version 330 core
            layout(location = 0) in vec3 a_Position;
            layout(location = 1) in vec4 a_Color;
            out vec3 v_Position;
            out vec4 v_Color;

            uniform mat4 u_ProjectionView;

            void main()
            {
                v_Position = a_Position;
                v_Color = a_Color;
                gl_Position = u_ProjectionView * vec4(a_Position, 1.0);
            }
        )";

        // fragment shader
        std::string triangle_fragment_shader = R"(
            #version 330 core
            layout(location = 0) out vec4 color;
            in vec3 v_Position;
            in vec4 v_Color;

            void main()
            {
                color = vec4(v_Position * 0.5 + 0.5, 1.0);
                color = v_Color;
            }
        )";

        m_triangle_shader =
            Hazel::Shader::create("triangle", triangle_vertex_shader, triangle_fragment_shader);

        // square render
        m_square_va = Hazel::VertexArray::create();

        // clang-format off
        float square_vertices[5 * 4] = {
            -0.75f, -0.75f, 0.0f, 0.0f, 0.0f,
            0.75f, -0.75f, 0.0f, 1.0f, 0.0f,
            0.75f, 0.75f, 0.0f, 1.0f, 1.0f,
            -0.75f, 0.75f, 0.0f, 0.0f, 1.0f
        };
        // clang-format on
        std::shared_ptr<Hazel::VertexBuffer> square_vertex_buffer;
        square_vertex_buffer =
            Hazel::VertexBuffer::create(square_vertices, sizeof(square_vertices));

        square_vertex_buffer->setLayout({{Hazel::ShaderDataType::Float3, "a_Position"},
                                         {Hazel::ShaderDataType::Float2, "a_TexCoord"}});
        m_square_va->addVertexBuffer(square_vertex_buffer);

        uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};  // 逆时针 绘制顺序
        std::shared_ptr<Hazel::IndexBuffer> square_index_buffer;
        square_index_buffer =
            Hazel::IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t));
        m_square_va->setIndexBuffer(square_index_buffer);

        // shader对象
        // vertex shader
        std::string square_vertex_shader = R"(
            #version 330 core
            layout(location = 0) in vec3 a_Position;
            out vec3 v_Position;

            uniform mat4 u_ProjectionView;
            uniform mat4 u_Transform;

            void main()
            {
                v_Position = a_Position;
                gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
            }
        )";

        // fragment shader
        std::string square_fragment_shader = R"(
            #version 330 core
            layout(location = 0) out vec4 color;
            in vec3 v_Position;

            uniform vec4 u_Color;

            void main()
            {
                // color = vec4(v_Position * 0.5 + 0.5, 1.0);
                // color = vec4(0.2, 0.3, 0.8, 1.0);
                color = u_Color;
            }
        )";

        m_square_shader =
            Hazel::Shader::create("square", square_vertex_shader, square_fragment_shader);

        // 读取文件创建shader
        auto texture_shader = m_texture_shader_lib.load("sandbox/assets/shaders/texture.glsl");

        // 加载纹理图像
        // 1. 加载棋盘纹理
        m_texture_b = Hazel::Texture2D::create("sandbox/assets/textures/Checkerboard.png");
        // 2. 加载前景图
        m_texture_f = Hazel::Texture2D::create("sandbox/assets/textures/namica.png");

        // 绑定 m_texture_shader, 指定插槽0为纹理上传位置
        texture_shader->bind();
        std::static_pointer_cast<Hazel::OpenGLShader>(texture_shader)
            ->uploadUniformInt("u_Texture", 0);
    }

    void onUpdate(Hazel::Timestep timestep) override
    {
        Hazel::RendererCommand::setClearColor({0.2f, 0.2f, 0.2f, 1.0f});
        Hazel::RendererCommand::clear();

        m_camera_controller.onUpdate(timestep);

        // HZ_INFO("当前帧间隔: {}s, {}ms", timestep.getSeconds(), timestep.getMilliseconds());

        // 简单让正方形动起来
        // if (Hazel::Input::isKeyPressed(HZ_KEY_J)) {
        //     m_square_pos.x -= m_square_speed * timestep;
        // } else if (Hazel::Input::isKeyPressed(HZ_KEY_L)) {
        //     m_square_pos.x += m_square_speed * timestep;
        // }
        // if (Hazel::Input::isKeyPressed(HZ_KEY_I)) {
        //     m_square_pos.y += m_square_speed * timestep;
        // } else if (Hazel::Input::isKeyPressed(HZ_KEY_K)) {
        //     m_square_pos.y -= m_square_speed * timestep;
        // }

        // glm::mat4 square_transform = glm::translate(glm::mat4{1.0f}, m_square_pos);

        Hazel::Renderer::beginScene(m_camera_controller.getCamera());

        // 渲染一个20 x 20的正方形网格
        glm::mat4 scale = glm::scale(glm::mat4{1.0f}, glm::vec3{0.1f});  // 缩放0.1倍

        m_square_shader->bind();
        std::static_pointer_cast<Hazel::OpenGLShader>(m_square_shader)
            ->uploadUniformFloat4("u_Color", m_square_color);

        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                glm::vec3 pos{j * 0.18f, i * 0.18f, 0.0f};
                glm::mat4 transform = glm::translate(glm::mat4{1.0f}, pos) * scale;
                Hazel::Renderer::submit(m_square_shader, m_square_va, transform);
            }
        }

        // 渲染一个用于绘制2d纹理的正方形
        glm::mat4 square_transform = glm::scale(glm::mat4{1.0f}, glm::vec3{1.0f});

        auto texture_shader = m_texture_shader_lib.get("texture");

        m_texture_b->bind();
        Hazel::Renderer::submit(texture_shader, m_square_va, square_transform);
        m_texture_f->bind();
        Hazel::Renderer::submit(texture_shader, m_square_va, square_transform);

        // 渲染三角形
        // Hazel::Renderer::submit(m_triangle_shader, m_triangle_va);
        Hazel::Renderer::endScene();
    }

    void onEvent(Hazel::Event& e) override
    {
        m_camera_controller.onEvent(e);
    }

    void onImGuiRender() override
    {
        ImGui::Begin("Settings");
        ImGui::ColorEdit4("Square Color", glm::value_ptr(m_square_color));
        ImGui::End();
    }

private:
    glm::vec4 m_square_color{1.0f};
    // glm::vec3 m_square_pos = glm::vec3{0.0f};
    // float m_square_speed = 0.1f;

private:
    Hazel::Ref<Hazel::Shader> m_triangle_shader;
    Hazel::Ref<Hazel::VertexArray> m_triangle_va;

    Hazel::Ref<Hazel::Shader> m_square_shader;
    Hazel::Ref<Hazel::VertexArray> m_square_va;

    Hazel::ShaderLibrary m_texture_shader_lib;
    Hazel::Ref<Hazel::Texture> m_texture_b;
    Hazel::Ref<Hazel::Texture> m_texture_f;

    Hazel::OrthoGraphicCameraController m_camera_controller;
};

class SandboxApp : public Hazel::Application
{
public:
    SandboxApp()
    {
        // pushLayer(new ExampleLayer{});
        pushLayer(new Sandbox2D{});
    }
    ~SandboxApp() {}
};

Hazel::Application* Hazel::createApplication()
{
    return new SandboxApp{};
}
