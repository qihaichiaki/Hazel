#pragma once

/**
 * 渲染高级类
 */

#include "renderer_command.h"
#include "shader.h"
#include "ortho_graphic_camera.h"

namespace Hazel
{

class HAZEL_API Renderer
{
public:
    static void init();

    /// @brief 渲染场景的开始
    /// @note 传入相机对象, 备份其投影视图矩阵, 方便用于后续上传数据的使用
    static void beginScene(const OrthoGraphicCamera& camera);

    /// @brief 上传顶点数组相关数据和shader
    /// @note 当前未实现渲染队列, 这里提交相当于实时渲染绘制数据
    static void submit(const Ref<Shader>& shader,
                       const Ref<VertexArray>& vertex_array,
                       const glm::mat4& transform = glm::mat4{1.0f});

    /// @brief 场景数据准备完毕
    static void endScene();

    /// @brief 返回当前渲染器的具体使用API类型
    static RendererAPI::API getRendererAPI()
    {
        return RendererAPI::getAPI();
    }

    /// @brief 调整渲染器的渲染缓冲区窗口大小
    static void onWindowResize(uint32_t width, uint32_t height);

private:
    struct SceneData
    {
        glm::mat4 projection_view_matrix;
    };

    static SceneData* m_scene_data;
};
}  // namespace Hazel