#include "hzpch.h"
#include "application.h"
#include "Hazel/Events/application_event.h"
#include "Hazel/input.h"
#include "Hazel/ImGui/imgui_layer.h"

#include <glad/glad.h>

namespace Hazel
{
Application* Application::s_instance = nullptr;

Application::Application()
{
    HZ_CORE_ASSERT(s_instance == nullptr, "application被创建多次!")
    s_instance = this;

    m_window = std::unique_ptr<Window>(Window::create());
    m_window->setEventCallBack(HZ_BIND_EVENT_FN(Application::onEvent));

    m_imgui_layer = new ImGuiLayer{};
    pushOverlay(m_imgui_layer);

    // triangle render
    m_triangle_va.reset(VertexArray::create());

    // clang-format off
    float triangle_vertices[3 * 7] = {
        -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
    };
    // clang-format on
    std::shared_ptr<VertexBuffer> triangle_vertex_buffer;
    triangle_vertex_buffer.reset(
        VertexBuffer::create(triangle_vertices, sizeof(triangle_vertices)));

    BufferLayer triangle_vertex_layer{{ShaderDataType::Float3, "a_Position"},
                                      {ShaderDataType::Float4, "a_Color"}};
    triangle_vertex_buffer->setLayout(triangle_vertex_layer);
    m_triangle_va->addVertexBuffer(triangle_vertex_buffer);

    uint32_t triangle_indices[3] = {0, 1, 2};  // 逆时针 绘制顺序
    std::shared_ptr<IndexBuffer> triangle_index_buffer;
    triangle_index_buffer.reset(
        IndexBuffer::create(triangle_indices, sizeof(triangle_indices) / sizeof(uint32_t)));
    m_triangle_va->setIndexBuffer(triangle_index_buffer);

    // shader对象
    // vertex shader
    std::string triangle_vertex_shader = R"(
        #version 330 core
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;
        out vec3 v_Position;
        out vec4 v_Color;

        void main()
        {
            v_Position = a_Position;
            v_Color = a_Color;
            gl_Position = vec4(a_Position, 1.0);
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

    m_triangle_shader.reset(new Shader{triangle_vertex_shader, triangle_fragment_shader});

    // square render
    m_square_va.reset(VertexArray::create());

    // clang-format off
    float square_vertices[4 * 3] = {
        -0.75f, -0.75f, 0.0f, 
        0.75f, -0.75f, 0.0f, 
        0.75f, 0.75f, 0.0f, 
        -0.75f, 0.75f, 0.0f
    };
    // clang-format on
    std::shared_ptr<VertexBuffer> square_vertex_buffer;
    square_vertex_buffer.reset(VertexBuffer::create(square_vertices, sizeof(square_vertices)));

    square_vertex_buffer->setLayout({{ShaderDataType::Float3, "a_Position"}});
    m_square_va->addVertexBuffer(square_vertex_buffer);

    uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};  // 逆时针 绘制顺序
    std::shared_ptr<IndexBuffer> square_index_buffer;
    square_index_buffer.reset(
        IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t)));
    m_square_va->setIndexBuffer(square_index_buffer);

    // shader对象
    // vertex shader
    std::string square_vertex_shader = R"(
        #version 330 core
        layout(location = 0) in vec3 a_Position;
        out vec3 v_Position;

        void main()
        {
            v_Position = a_Position;
            gl_Position = vec4(a_Position, 1.0);
        }
    )";

    // fragment shader
    std::string square_fragment_shader = R"(
        #version 330 core
        layout(location = 0) out vec4 color;
        in vec3 v_Position;

        void main()
        {
            color = vec4(v_Position * 0.5 + 0.5, 1.0);
        }
    )";

    m_square_shader.reset(new Shader{square_vertex_shader, square_fragment_shader});
}

Application::~Application() {}

void Application::run()
{
    while (m_running) {
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // 渲染正方形
        m_square_shader->bind();
        m_square_va->bind();
        glDrawElements(GL_TRIANGLES, m_square_va->getIndexBuffer()->getCount(), GL_UNSIGNED_INT,
                       nullptr);

        // 渲染三角形
        m_triangle_shader->bind();
        m_triangle_va->bind();
        glDrawElements(GL_TRIANGLES, m_triangle_va->getIndexBuffer()->getCount(), GL_UNSIGNED_INT,
                       nullptr);

        // 层级从左往右更新/渲染
        for (auto& layer : m_layer_stack) {
            layer->onUpdate();
        }
        // imgui 更新层
        m_imgui_layer->begin();
        for (auto& layer : m_layer_stack) {
            layer->onImGuiRender();
        }
        m_imgui_layer->end();

        m_window->onUpdate();
    }
}

void Application::onEvent(Event& event)
{
    EventDispatcher dispatcher{event};
    dispatcher.dispatch<WindowCloseEvent>(HZ_BIND_EVENT_FN(Application::onWindowClosed));

    for (auto layer_it = m_layer_stack.end(); layer_it != m_layer_stack.begin();) {
        (*(--layer_it))->onEvent(event);
        if (event.isHandled()) {
            break;
        }
    }
}

bool Application::onWindowClosed(WindowCloseEvent&)
{
    m_running = false;
    return true;
}

void Application::pushLayer(Layer* layer)
{
    m_layer_stack.pushLayer(layer);
}

void Application::pushOverlay(Layer* overly)
{
    m_layer_stack.pushOverlay(overly);
}

void Application::popLayer(Layer* layer)
{
    m_layer_stack.popLayer(layer);
}

void Application::popOverlay(Layer* overly)
{
    m_layer_stack.popOverlay(overly);
}

void popLayer(Layer* layer)
{
    layer->onDetach();
    // TODO: layer pop待实现
}

void popOverlay(Layer* overly)
{
    overly->onDetach();
    // TODO: overly pop待实现
}

}  // namespace Hazel
