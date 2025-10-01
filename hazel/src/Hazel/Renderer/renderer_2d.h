#pragma once

/**
 * 2d渲染器的实现
 */

#include "ortho_graphic_camera.h"
#include "texture.h"

namespace Hazel
{
class Renderer2D
{
public:
    /// @brief 2d渲染器初始化, 准备一些固有的顶点数组, 相关buffer, 预制的shader
    HAZEL_API static void init();
    /// @brief 2d渲染器释放资源, 一些申请的gpu的资源进行释放
    HAZEL_API static void shutdown();

    /// @brief 2d渲染器开始场景. 准备/上传当前绘制场景公共的数据
    /// @param camera 正交摄像机对象, 准备上传其投影矩阵和视图矩阵数据
    HAZEL_API static void beginScene(const OrthoGraphicCamera& camera);
    /// @brief 2d渲染器结束场景, 当前实时渲染没有做任何动作
    HAZEL_API static void endScene();

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
};
}  // namespace Hazel