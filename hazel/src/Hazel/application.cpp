#include "hzpch.h"
#include "application.h"
#include "Hazel/Events/application_event.h"
#include "Hazel/input.h"
#include "Hazel/ImGui/imgui_layer.h"

#include <glad/glad.h>

static unsigned int s_vertex_arr, s_vertex_buffer, s_index_buffer;

// 临时使用
static GLenum shaderDataTypeToOpenGLBaseType(Hazel::ShaderDataType type)
{
    switch (type) {
        case Hazel::ShaderDataType::Float:
        case Hazel::ShaderDataType::Float2:
        case Hazel::ShaderDataType::Float3:
        case Hazel::ShaderDataType::Float4:
            return GL_FLOAT;
        case Hazel::ShaderDataType::Int:
        case Hazel::ShaderDataType::Int2:
        case Hazel::ShaderDataType::Int3:
        case Hazel::ShaderDataType::Int4:
            return GL_INT;
        case Hazel::ShaderDataType::Mat3:
        case Hazel::ShaderDataType::Mat4:
            return GL_FLOAT;
    }

    HZ_CORE_ASSERT(false, "未知的ShaderData类型!");
    return 0;
}
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

    // 示例: 使用opengl原生接口渲染三角形
    // vao
    glGenVertexArrays(1, &s_vertex_arr);
    glBindVertexArray(s_vertex_arr);

    // clang-format off
    float vertices[3 * 7] = {
        -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
    };
    // clang-format on
    m_vertex_buffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));
    BufferLayer vertex_layer{{ShaderDataType::Float3, "a_Position"},
                             {ShaderDataType::Float4, "a_Color"}};

    m_vertex_buffer->setLayout(vertex_layer);
    int index = 0;
    for (const auto& element : m_vertex_buffer->getLayout()) {
        // 为shader启动顶点属性
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, element.getComponentCount(),
                              shaderDataTypeToOpenGLBaseType(element.type),
                              element.normalized ? GL_TRUE : GL_FALSE, vertex_layer.getStride(),
                              reinterpret_cast<const void*>((uint16_t)element.offset));
        index++;
    }

    uint32_t indices[3] = {0, 1, 2};  // 逆时针 绘制顺序
    m_index_buffer.reset(IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));

    // shader对象
    // vertex shader
    std::string vertex_shader = R"(
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
    std::string fragment_shader = R"(
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

    m_shader.reset(new Shader{vertex_shader, fragment_shader});
}

Application::~Application() {}

void Application::run()
{
    while (m_running) {
        glClearColor(0.2f, 0.2f, 0.2f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        m_shader->bind();
        glBindVertexArray(s_vertex_arr);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
        m_shader->unBind();

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
