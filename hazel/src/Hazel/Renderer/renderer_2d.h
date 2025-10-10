#pragma once

/**
 * 2d渲染器的实现
 */

#include "Hazel/Renderer/ortho_graphic_camera.h"
#include "Hazel/Renderer/camera.h"
#include "Hazel/Renderer/texture.h"

namespace Hazel
{
class Renderer2D
{
public:
    /// @brief 2d渲染器初始化, 准备一些固有的顶点数组, 相关buffer, 预制的shader
    HAZEL_API static void init();
    /// @brief 2d渲染器释放资源, 一些申请的gpu的资源进行释放
    HAZEL_API static void shutdown();

    /// @brief 2d渲染器开始场景
    /// @param camera 运行时相机
    /// @param transform 相机的transform
    HAZEL_API static void beginScene(const Camera& camera, const glm::mat4& transform);
    /// @brief 2d渲染器开始场景. 准备/上传当前绘制场景公共的数据
    /// @param camera 正交摄像机对象, 准备上传其投影矩阵和视图矩阵数据
    HAZEL_API static void beginScene(const OrthoGraphicCamera& camera);  // TODO: remove
    /// @brief 2d渲染器刷新场景->绘制数据
    HAZEL_API static void flush();
    /// @brief 2d渲染器结束场景
    HAZEL_API static void endScene();

    /// @brief 提交根据transform渲染的带颜色的四边形
    /// @param transform 当前图形的基于世界的transform
    /// @param color 图形颜色
    HAZEL_API static void drawQuad(const glm::mat4& transform, const glm::vec4& color);

    /// @brief 提交根据transform渲染的带颜色, 平铺因子的纹理四边形
    /// @param transform 当前纹理的基于世界的transform
    /// @param texture 2d纹理对象
    /// @param tint_color 平铺因子
    /// @param tiling_factor 混合颜色
    HAZEL_API static void drawQuad(const glm::mat4& transform,
                                   const Ref<Texture2D>& texture,
                                   const glm::vec4& tint_color = glm::vec4{1.0f},
                                   float tiling_factor = 1.0f);

    /// 绘制非旋转的四边形

    /// @brief 根据给出信息实时绘制一个四边形
    /// @param position 当前四边形的世界坐标(2d渲染中z标识图层高度)
    /// @param size 当前四边形的大小
    /// @param color 当前四边形的颜色
    HAZEL_API static void drawQuad(const glm::vec3& position,
                                   const glm::vec2& size,
                                   const glm::vec4& color);
    /// @brief 根据给出信息实时绘制一个四边形
    /// @param position 当前四边形的世界坐标(z图层默认为0)
    /// @param size 当前四边形的大小
    /// @param color 当前四边形的颜色
    HAZEL_API static void drawQuad(const glm::vec2& position,
                                   const glm::vec2& size,
                                   const glm::vec4& color);

    /// @brief 在给出的四边形上实时绘制出一个加载的纹理
    /// @param position 当前四边形的世界坐标(2d渲染中z标识图层高度)
    /// @param size 当前四边形的大小
    /// @param texture 2d纹理数据
    /// @param tint_color 混合颜色
    /// @param tiling_factor 平铺因子
    HAZEL_API static void drawQuad(const glm::vec3& position,
                                   const glm::vec2& size,
                                   const Ref<Texture2D>& texture,
                                   const glm::vec4& tint_color = glm::vec4{1.0f},
                                   float tiling_factor = 1.0f);
    /// @brief 在给出的四边形上实时绘制出一个加载的纹理
    /// @param position 当前四边形的世界坐标(z图层默认为0)
    /// @param size 当前四边形的大小
    /// @param texture 2d纹理数据
    /// @param tint_color 混合颜色
    /// @param tiling_factor 平铺因子
    HAZEL_API static void drawQuad(const glm::vec2& position,
                                   const glm::vec2& size,
                                   const Ref<Texture2D>& texture,
                                   const glm::vec4& tint_color = glm::vec4{1.0f},
                                   float tiling_factor = 1.0f);

    /// 绘制旋转的四边形

    /// @brief 根据给出信息实时绘制一个四边形
    /// @param position 当前四边形的世界坐标(2d渲染中z标识图层高度)
    /// @param size 当前四边形的大小
    /// @param rotation 沿着z轴旋转的弧度值
    /// @param color 当前四边形的颜色
    HAZEL_API static void drawRotatedQuad(const glm::vec3& position,
                                          const glm::vec2& size,
                                          float rotation,
                                          const glm::vec4& color);
    /// @brief 根据给出信息实时绘制一个四边形
    /// @param position 当前四边形的世界坐标(z图层默认为0)
    /// @param size 当前四边形的大小
    /// @param rotation 沿着z轴旋转的弧度值
    /// @param color 当前四边形的颜色
    HAZEL_API static void drawRotatedQuad(const glm::vec2& position,
                                          const glm::vec2& size,
                                          float rotation,
                                          const glm::vec4& color);

    /// @brief 在给出的四边形上实时绘制出一个加载的纹理
    /// @param position 当前四边形的世界坐标(2d渲染中z标识图层高度)
    /// @param size 当前四边形的大小
    /// @param rotation 沿着z轴旋转的弧度值
    /// @param texture 2d纹理数据
    /// @param tint_color 混合颜色
    /// @param tiling_factor 平铺因子
    HAZEL_API static void drawRotatedQuad(const glm::vec3& position,
                                          const glm::vec2& size,
                                          float rotation,
                                          const Ref<Texture2D>& texture,
                                          const glm::vec4& tint_color = glm::vec4{1.0f},
                                          float tiling_factor = 1.0f);
    /// @brief 在给出的四边形上实时绘制出一个加载的纹理
    /// @param position 当前四边形的世界坐标(z图层默认为0)
    /// @param size 当前四边形的大小
    /// @param rotation 沿着z轴旋转的弧度值
    /// @param texture 2d纹理数据
    /// @param tint_color 混合颜色
    /// @param tiling_factor 平铺因子
    HAZEL_API static void drawRotatedQuad(const glm::vec2& position,
                                          const glm::vec2& size,
                                          float rotation,
                                          const Ref<Texture2D>& texture,
                                          const glm::vec4& tint_color = glm::vec4{1.0f},
                                          float tiling_factor = 1.0f);

    // Stats
    struct HAZEL_API Statistics
    {
        uint32_t DrawCalls = 0;
        uint32_t QuadCount = 0;

        uint32_t getTotalVertexCount()
        {
            return QuadCount * 4;
        }
        uint32_t getTotalIndexCount()
        {
            return QuadCount * 6;
        }
    };

    /// @brief 获取当前渲染器的调用状态
    HAZEL_API static Statistics getStats();

    /// @brief 重置当前渲染器的调用状态
    HAZEL_API static void resetStats();

private:
    static void flushAndReset();
};
}  // namespace Hazel