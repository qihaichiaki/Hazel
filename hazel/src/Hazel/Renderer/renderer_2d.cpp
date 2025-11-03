#include "hzpch.h"
#include "renderer_2d.h"

#include "vertex_array.h"
#include "shader.h"
#include "renderer_command.h"
#include "Hazel/Renderer/uniform_buffer.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel
{

// 四边形顶点数据
struct QuadVertex
{
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoord;
    float TexId;
    float TilingFactor;

    int EntityID;
};

// 圆顶点数据
struct CircleVertex
{
    glm::vec3 WorldPosition;
    glm::vec3 LocalPosition;  // 局部世界坐标, 类似于UV?
    glm::vec4 Color;
    float Thickness;
    float Fade;

    int EntityID;
};

struct Renderer2DData
{
    static const uint32_t MaxQuads = 10000;               // 允许的最大四边形绘制数量
    static const uint32_t MaxVertices = MaxQuads * 4;     // 允许的最大绘制顶点个数
    static const uint32_t MaxIndices = MaxQuads * 3 * 2;  // 允许的最大绘制索引个数
    static const uint32_t MaxTextureSolts = 32;           // 允许的最大绑定纹理槽数量

    // Quad
    Ref<VertexArray> QuadVertexArray;    // 四边形的基础顶点数组对象
    Ref<VertexBuffer> QuadVertexBuffer;  // 四边形顶点缓冲区
    Ref<Shader> QuadTextureShader;       // 可渲染纹理的四边形shader
    Ref<Texture2D> WhiteTexure;          // 不渲染图片的清空下使用白色纹理

    QuadVertex* QuadVertexBufferBase = nullptr;  // 顶点数组实际准备数据
    QuadVertex* QuadVertexBufferPtr = nullptr;   // 顶点数组当前顶点数据
    uint32_t QuadCount = 0;                      // 四边形绘制个数

    std::array<Ref<Texture2D>, MaxTextureSolts> QuadTextureSolts;
    uint32_t QuadTextureSoltCount = 0;  // 四边形槽占用数

    // Circle
    Ref<VertexArray> CircleVertexArray;
    Ref<VertexBuffer> CircleVertexBuffer;
    Ref<Shader> CircleShader;

    CircleVertex* CircleVertexBufferBase = nullptr;
    CircleVertex* CircleVertexBufferPtr = nullptr;
    uint32_t CircleCount = 0;

    Renderer2D::Statistics Stats;

    const glm::vec4 RefPositions[4] = {
        glm::vec4{-0.5f, -0.5f, 0.0f, 1.0f}, glm::vec4{0.5f, -0.5f, 0.0f, 1.0f},
        glm::vec4{0.5f, 0.5f, 0.0f, 1.0f},
        glm::vec4{-0.5f, 0.5f, 0.0f, 1.0f}};  // 参照坐标, 坐标的transform依据此进行变换
    const glm::vec2 RefTexCoords[4] = {glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f},
                                       glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f}};  // 参照UV坐标

    struct CameraData
    {
        glm::mat4 ViewProjection;
    };
    CameraData CameraBuffer;
    Ref<UniformBuffer> CameraUniformBuffer;
};

static Renderer2DData s_data;

void Renderer2D::init()
{
    HZ_PROFILE_FUNCTION();

    // ====== Quad ======
    // 1. 创建quad顶点数组对象
    s_data.QuadVertexArray = VertexArray::create();

    // 2. 创建quad固定大小, 非固定数据顶点缓冲区
    s_data.QuadVertexBuffer =
        VertexBuffer::create(Renderer2DData::MaxVertices * sizeof(QuadVertex));
    // 3. quad设置布局 -> vertex shader如何读取顶点数据
    s_data.QuadVertexBuffer->setLayout({{ShaderDataType::Float3, "a_Position"},
                                        {ShaderDataType::Float4, "a_Color"},
                                        {ShaderDataType::Float2, "a_TexCoord"},
                                        {ShaderDataType::Float, "a_TexId"},
                                        {ShaderDataType::Float, "a_TilingFactor"},
                                        {ShaderDataType::Int, "a_EntityID"}});
    // 4. 设置到顶点数组对象中去
    s_data.QuadVertexArray->addVertexBuffer(s_data.QuadVertexBuffer);

    // 创建顶点数据
    s_data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MaxVertices];

    // 5. 创建索引数组, 标识顶点读取顺序
    uint32_t* index_buffer = new uint32_t[s_data.MaxIndices];
    // 设置四边形的索引顺序
    uint32_t offset = 0;
    for (uint32_t i = 0; i < s_data.MaxIndices; i += 6) {
        index_buffer[i + 0] = offset + 0;
        index_buffer[i + 1] = offset + 1;
        index_buffer[i + 2] = offset + 2;

        index_buffer[i + 3] = offset + 2;
        index_buffer[i + 4] = offset + 3;
        index_buffer[i + 5] = offset + 0;
        offset += 4;
    }
    auto quad_index_buffer = IndexBuffer::create(index_buffer, s_data.MaxIndices);
    // 6. 设置到到顶点数组对象中去
    s_data.QuadVertexArray->setIndexBuffer(quad_index_buffer);

    // 7. 读取纹理shader
    s_data.QuadTextureShader = Shader::create("assets/shaders/Renderer2D_QuadTexture.glsl");

    // 8. 创建单位白色纹理, 用于渲染纯色方块
    s_data.WhiteTexure = Texture2D::create(1, 1);
    uint32_t data = 0xffffffff;  // 纯白
    s_data.WhiteTexure->setData(&data, sizeof(data));

    s_data.QuadTextureSolts[0] = s_data.WhiteTexure;  // 0号位绑定纯白纹理
    s_data.QuadTextureSoltCount = 1;

    s_data.QuadTextureShader->bind();
    int texture_solts[s_data.MaxTextureSolts];
    for (int i = 0; i < s_data.MaxTextureSolts; ++i) {
        texture_solts[i] = i;
    }
    s_data.QuadTextureShader->setIntArray("u_Textures", texture_solts, s_data.MaxTextureSolts);
    // ============

    // ====== Circle ======
    s_data.CircleVertexArray = VertexArray::create();

    // TODO: 圆的绘制上限借用MaxQuads, 后续可以自由调整
    s_data.CircleVertexBuffer =
        VertexBuffer::create(Renderer2DData::MaxVertices * sizeof(QuadVertex));

    s_data.CircleVertexBuffer->setLayout({{ShaderDataType::Float3, "a_WorldPosition"},
                                          {ShaderDataType::Float3, "a_LocalPosition"},
                                          {ShaderDataType::Float4, "a_Color"},
                                          {ShaderDataType::Float, "a_Thickness"},
                                          {ShaderDataType::Float, "a_Fade"},
                                          {ShaderDataType::Int, "a_EntityID"}});
    s_data.CircleVertexArray->addVertexBuffer(s_data.CircleVertexBuffer);

    s_data.CircleVertexBufferBase = new CircleVertex[Renderer2DData::MaxVertices];

    auto circle_index_buffer = IndexBuffer::create(index_buffer, s_data.MaxIndices);
    s_data.CircleVertexArray->setIndexBuffer(circle_index_buffer);

    s_data.CircleShader = Shader::create("assets/shaders/Renderer2D_Circle.glsl");
    // ============

    s_data.CameraUniformBuffer = UniformBuffer::create(sizeof(Renderer2DData::CameraData), 0);
    delete[] index_buffer;
}

void Renderer2D::shutdown()
{
    HZ_PROFILE_FUNCTION();

    delete[] s_data.QuadVertexBufferBase;
    s_data.QuadVertexBufferBase = nullptr;
}

// 重置绘制
static inline void startBatch()
{
    // 四边形绘制重置
    s_data.QuadCount = 0;
    s_data.QuadVertexBufferPtr = s_data.QuadVertexBufferBase;

    // 绑定纹理槽重置
    s_data.QuadTextureSoltCount = 1;  // 0号始终由白色纹理占用

    // 圆形绘制重置
    s_data.CircleCount = 0;
    s_data.CircleVertexBufferPtr = s_data.CircleVertexBufferBase;
}

void Renderer2D::beginScene(const EditorCamera& camera)
{
    // 上传pv矩阵
    s_data.CameraBuffer.ViewProjection = camera.getViewProjection();
    s_data.CameraUniformBuffer->setData(&s_data.CameraBuffer, sizeof(Renderer2DData::CameraBuffer));

    startBatch();
}

void Renderer2D::beginScene(const Camera& camera, const glm::mat4& transform)
{
    // 上传pv矩阵
    auto projection_view_matrix =
        camera.getProjection() * glm::inverse(transform);  //  transform需要逆置
    s_data.CameraBuffer.ViewProjection = projection_view_matrix;
    s_data.CameraUniformBuffer->setData(&s_data.CameraBuffer, sizeof(Renderer2DData::CameraBuffer));

    startBatch();
}

void Renderer2D::beginScene(const OrthoGraphicCamera& camera)
{
    HZ_PROFILE_FUNCTION();

    // 上传pv矩阵
    s_data.CameraBuffer.ViewProjection = camera.getProjectionViewMatrix();
    s_data.CameraUniformBuffer->setData(&s_data.CameraBuffer, sizeof(Renderer2DData::CameraBuffer));

    startBatch();
}

void Renderer2D::flush()
{
    if (s_data.QuadCount > 0) {
        // 上传顶点数据
        uint32_t size = (uint32_t)((uint8_t*)s_data.QuadVertexBufferPtr -
                                   (uint8_t*)s_data.QuadVertexBufferBase);
        s_data.QuadVertexBuffer->setData(s_data.QuadVertexBufferBase, size);

        // 绑定纹理槽
        for (uint32_t i = 0; i < s_data.QuadTextureSoltCount; ++i) {
            s_data.QuadTextureSolts[i]->bind(i);
        }

        s_data.QuadTextureShader->bind();
        RendererCommand::drawIndexed(s_data.QuadVertexArray, s_data.QuadCount * 6);

        s_data.Stats.DrawCalls++;
    }

    if (s_data.CircleCount > 0) {
        uint32_t size = (uint32_t)((uint8_t*)s_data.CircleVertexBufferPtr -
                                   (uint8_t*)s_data.CircleVertexBufferBase);
        s_data.CircleVertexBuffer->setData(s_data.CircleVertexBufferBase, size);

        s_data.CircleShader->bind();
        RendererCommand::drawIndexed(s_data.CircleVertexArray, s_data.CircleCount * 6);

        s_data.Stats.DrawCalls++;
    }
}

void Renderer2D::endScene()
{
    HZ_PROFILE_FUNCTION();

    // 绘制数据
    flush();
}

void Renderer2D::flushAndReset()
{
    endScene();
    startBatch();
}

void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color, int entity_id)
{
    HZ_PROFILE_FUNCTION();

    if (s_data.QuadCount >= Renderer2DData::MaxQuads) {
        // 超出限制, 重置flush
        flushAndReset();
    }

    const float white_texure_id = 0.0f;
    const float tiling_factor = 1.0f;
    // 多批次渲染, 准备数据
    // 3 ----- 2
    // |   /   |
    // 0 ----- 1

    for (int i = 0; i < 4; ++i) {
        s_data.QuadVertexBufferPtr->Position = transform * s_data.RefPositions[i];
        s_data.QuadVertexBufferPtr->Color = color;
        s_data.QuadVertexBufferPtr->TexCoord = s_data.RefTexCoords[i];
        s_data.QuadVertexBufferPtr->TexId = white_texure_id;
        s_data.QuadVertexBufferPtr->TilingFactor = tiling_factor;
        s_data.QuadVertexBufferPtr->EntityID = entity_id;
        s_data.QuadVertexBufferPtr++;
    }

    s_data.QuadCount++;
    s_data.Stats.QuadCount++;
}

void Renderer2D::drawQuad(const glm::mat4& transform,
                          const Ref<Texture2D>& texture,
                          const glm::vec4& tint_color,
                          float tiling_factor,
                          int entity_id)
{
    HZ_PROFILE_FUNCTION();

    if (s_data.QuadCount >= Renderer2DData::MaxQuads) {
        // 超出限制, 重置flush
        flushAndReset();
    }

    // 寻找纹理槽index
    float texure_id = 0.0f;
    for (uint32_t i = 1; i < s_data.QuadTextureSoltCount; ++i) {
        if (s_data.QuadTextureSolts[i]->isEqual(*texture)) {
            texure_id = (float)i;
            break;
        }
    }
    if (texure_id == 0.0f) {
        if (s_data.QuadTextureSoltCount >= s_data.MaxTextureSolts) {
            flushAndReset();
        }
        texure_id = (float)s_data.QuadTextureSoltCount;
        s_data.QuadTextureSolts[s_data.QuadTextureSoltCount++] = texture;
    }

    for (int i = 0; i < 4; ++i) {
        s_data.QuadVertexBufferPtr->Position = transform * s_data.RefPositions[i];
        s_data.QuadVertexBufferPtr->Color = tint_color;
        s_data.QuadVertexBufferPtr->TexCoord = s_data.RefTexCoords[i];
        s_data.QuadVertexBufferPtr->TexId = texure_id;
        s_data.QuadVertexBufferPtr->TilingFactor = tiling_factor;
        s_data.QuadVertexBufferPtr->EntityID = entity_id;
        s_data.QuadVertexBufferPtr++;
    }
    s_data.QuadCount++;
    s_data.Stats.QuadCount++;
}

void Renderer2D::drawCircle(
    const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entity_id)
{
    HZ_PROFILE_FUNCTION();

    // TODO: 超出限制, 重置flush
    // if (s_data.QuadCount >= Renderer2DData::MaxQuads) {
    // flushAndReset();
    // }

    for (int i = 0; i < 4; ++i) {
        s_data.CircleVertexBufferPtr->WorldPosition = transform * s_data.RefPositions[i];
        s_data.CircleVertexBufferPtr->LocalPosition = 2.0f * s_data.RefPositions[i];
        s_data.CircleVertexBufferPtr->Color = color;
        s_data.CircleVertexBufferPtr->Thickness = thickness;
        s_data.CircleVertexBufferPtr->Fade = fade;
        s_data.CircleVertexBufferPtr->EntityID = entity_id;
        s_data.CircleVertexBufferPtr++;
    }
    s_data.CircleCount++;
    s_data.Stats.QuadCount++;
}

void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
    // 准备变换矩阵
    glm::mat4 transform = glm::translate(glm::mat4{1.0f}, position) *
                          glm::scale(glm::mat4{1.0f}, {size.x, size.y, 1.0f});
    drawQuad(transform, color);
}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
    drawQuad({position.x, position.y, 0.0f}, size, color);
}

void Renderer2D::drawQuad(const glm::vec3& position,
                          const glm::vec2& size,
                          const Ref<Texture2D>& texture,
                          const glm::vec4& tint_color,
                          float tiling_factor)
{
    glm::mat4 transform = glm::translate(glm::mat4{1.0f}, position) *
                          glm::scale(glm::mat4{1.0f}, glm::vec3{size.x, size.y, 1.0f});
    drawQuad(transform, texture, tint_color, tiling_factor);
}

void Renderer2D::drawQuad(const glm::vec2& position,
                          const glm::vec2& size,
                          const Ref<Texture2D>& texture,
                          const glm::vec4& tint_color,
                          float tiling_factor)
{
    drawQuad(glm::vec3{position.x, position.y, 0.0f}, size, texture, tint_color, tiling_factor);
}

//// 旋转绘制, 多计算一次旋转矩阵
void Renderer2D::drawRotatedQuad(const glm::vec3& position,
                                 const glm::vec2& size,
                                 float rotation,
                                 const glm::vec4& color)
{
    glm::mat4 transform = glm::translate(glm::mat4{1.0f}, position) *
                          glm::rotate(glm::mat4{1.0f}, rotation, glm::vec3{0.0f, 0.0f, 1.0f}) *
                          glm::scale(glm::mat4{1.0f}, glm::vec3{size.x, size.y, 1.0f});

    drawQuad(transform, color);
}

void Renderer2D::drawRotatedQuad(const glm::vec2& position,
                                 const glm::vec2& size,
                                 float rotation,
                                 const glm::vec4& color)
{
    drawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, color);
}

void Renderer2D::drawRotatedQuad(const glm::vec3& position,
                                 const glm::vec2& size,
                                 float rotation,
                                 const Ref<Texture2D>& texture,
                                 const glm::vec4& tint_color,
                                 float tiling_factor)
{
    glm::mat4 transform = glm::translate(glm::mat4{1.0f}, position) *
                          glm::rotate(glm::mat4{1.0f}, rotation, glm::vec3{0.0f, 0.0f, 1.0f}) *
                          glm::scale(glm::mat4{1.0f}, glm::vec3{size.x, size.y, 1.0f});
    drawQuad(transform, texture, tint_color, tiling_factor);
}

void Renderer2D::drawRotatedQuad(const glm::vec2& position,
                                 const glm::vec2& size,
                                 float rotation,
                                 const Ref<Texture2D>& texture,
                                 const glm::vec4& tint_color,
                                 float tiling_factor)
{
    drawRotatedQuad(glm::vec3{position.x, position.y, 0.0f}, size, rotation, texture, tint_color,
                    tiling_factor);
}

Renderer2D::Statistics Renderer2D::getStats()
{
    return s_data.Stats;
}

void Renderer2D::resetStats()
{
    s_data.Stats.DrawCalls = 0;
    s_data.Stats.QuadCount = 0;
}
}  // namespace Hazel