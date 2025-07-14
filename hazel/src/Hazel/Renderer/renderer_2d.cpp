#include "hzpch.h"
#include "renderer_2d.h"

#include "vertex_array.h"
#include "shader.h"
#include "renderer_command.h"

#include <glm/gtc/matrix_transform.hpp>

// TODO: 临时直接转为opengl shader
#include "PlatForm/OpenGL/opengl_shader.h"

namespace Hazel
{

struct Renderer2DStorage
{
    Ref<VertexArray> QuadVertexArray;  // 四边形的基础顶点数组对象
    Ref<Shader> FlatColorShader;       // 平面颜色shader
};

static Renderer2DStorage* s_data = nullptr;

void Renderer2D::init()
{
    s_data = new Renderer2DStorage{};

    // 1. 创建quad顶点数组对象
    s_data->QuadVertexArray = VertexArray::create();

    // 2. 创建quad基础vertex buffer
    // clang-format off
    float quad_vertices[4 * 3] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };
    // clang-format on
    auto quad_vertex_buffer = VertexBuffer::create(quad_vertices, sizeof(quad_vertices));
    // 3. quad设置布局 -> vertex shader如何读取顶点数据
    quad_vertex_buffer->setLayout({{ShaderDataType::Float3, "a_Position"}});
    // 4. 设置到顶点数组对象中去
    s_data->QuadVertexArray->addVertexBuffer(quad_vertex_buffer);

    // 5. 创建索引数组, 标识顶点读取顺序
    uint32_t index_buffer[6] = {0, 1, 2, 2, 3, 0};
    auto quad_index_buffer = IndexBuffer::create(index_buffer, 6);
    // 6. 设置到到顶点数组对象中去
    s_data->QuadVertexArray->setIndexBuffer(quad_index_buffer);

    // 7. 读取平面颜色shader
    s_data->FlatColorShader = Shader::create("sandbox/assets/shaders/flatColor.glsl");
}

void Renderer2D::shutdown()
{
    delete s_data;
    s_data = nullptr;
}

void Renderer2D::beginScene(const OrthoGraphicCamera& camera)
{
    // 上传pv矩阵
    s_data->FlatColorShader->bind();
    std::static_pointer_cast<OpenGLShader>(s_data->FlatColorShader)
        ->uploadUniformMat4("u_ProjectionView", camera.getProjectionViewMatrix());
}

void Renderer2D::endScene() {}

void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
    // 1. 处理transform 矩阵 -> 对象当前的模型矩阵
    glm::mat4 transform = glm::translate(glm::mat4{1.0f}, position) *
                          glm::scale(glm::mat4{1.0f}, glm::vec3{size.x, size.y, 1.0f});

    // 2. 上传transform, color 矩阵到shader中去
    s_data->FlatColorShader->bind();
    std::static_pointer_cast<OpenGLShader>(s_data->FlatColorShader)
        ->uploadUniformMat4("u_Transform", transform);
    std::static_pointer_cast<OpenGLShader>(s_data->FlatColorShader)
        ->uploadUniformFloat4("u_Color", color);

    // 3. 调用渲染命令进行实时绘制
    s_data->QuadVertexArray->bind();
    RendererCommand::drawIndexed(s_data->QuadVertexArray);
}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
    drawQuad({position.x, position.y, 0.0f}, size, color);
}
}  // namespace Hazel