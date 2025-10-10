#pragma once

/**
 * 场景摄像机, 运行时相机派生而来, 方便实现各个功能
 */

#include "Hazel/Renderer/camera.h"

namespace Hazel
{

class SceneCamera : public Camera
{
public:
    HAZEL_API SceneCamera();
    HAZEL_API ~SceneCamera();

    /// @brief 设置正交相机
    /// @param orthographic_size 正交相机视角范围
    /// @param z_near z深度near
    /// @param z_far z深度far
    HAZEL_API void setOrthographic(float orthographic_size, float z_near, float z_far);

    /// @brief 设置正交相机范围大小
    HAZEL_API void setOrthographicSize(float orthographic_size);
    /// @brief 获取正交相机范围大小
    HAZEL_API float getOrthographicSize() const
    {
        return m_orthographic_size;
    }

    /// @brief 设置相机的可视宽度和高度
    HAZEL_API void setViewportSize(uint32_t width, uint32_t height);

private:
    /// @brief 计算投影矩阵
    void recalculateProjection();

private:
    float m_orthographic_size = 10.0f;
    float m_orthographic_near = -1.0f;
    float m_orthographic_far = 1.0f;
    float m_aspect_ratio = 0.0f;
};

}  // namespace Hazel