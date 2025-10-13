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
    enum class ProjectionType { Perspection = 0, Orthographic = 1 };

public:
    HAZEL_API SceneCamera();
    HAZEL_API ~SceneCamera();

    /// @brief 获取当前场景相机的投影类型
    HAZEL_API ProjectionType getProjectionType() const
    {
        return m_projection_type;
    }
    /// @brief 设置当前场景相机的投影类型
    HAZEL_API void setProjectionType(ProjectionType projection_type);

    /// @brief 设置透视相机垂直视野的弧度值
    HAZEL_API void setPerspectionVerticalFOV(float perspective_FOV);  // 设置透视垂直视野
    /// @brief 获取透视相机垂直视野的弧度值
    HAZEL_API float getPerspectionVerticalFOV() const
    {
        return m_perspective_fov;
    }
    /// @brief 设置透视相机裁剪空间近点值
    HAZEL_API void setPerspectiveNearClip(float perspective_near);
    /// @brief 获取透视相机裁剪空间近点值
    HAZEL_API float getPerspectiveNearClip() const
    {
        return m_perspective_near;
    }
    /// @brief 设置透视相机裁剪空间远点值
    HAZEL_API void setPerspectiveFarClip(float perspective_far);
    /// @brief 获取透视相机裁剪空间远点值
    HAZEL_API float getPerspectiveFarClip() const
    {
        return m_perspective_far;
    }

    /// @brief 设置正交相机范围大小
    HAZEL_API void setOrthographicSize(float orthographic_size);
    /// @brief 获取正交相机范围大小
    HAZEL_API float getOrthographicSize() const
    {
        return m_orthographic_size;
    }
    /// @brief 设置正交相机裁剪空间近点值
    HAZEL_API void setOrthographicNearClip(float orthographic_near);
    /// @brief 获取正交相机裁剪空间近点值
    HAZEL_API float getOrthographicNearClip() const
    {
        return m_orthographic_near;
    }
    /// @brief 设置正交相机裁剪空间远点值
    HAZEL_API void setOrthographicFarClip(float orthographic_far);
    /// @brief 获取正交相机裁剪空间远点值
    HAZEL_API float getOrthographicFarClip() const
    {
        return m_orthographic_far;
    }

    /// @brief 设置相机的可视宽度和高度
    HAZEL_API void setViewportSize(uint32_t width, uint32_t height);

private:
    /// @brief 计算投影矩阵
    void recalculateProjection();

private:
    // 投影类型, 因为是2d引擎, 所以优先正交投影
    ProjectionType m_projection_type = ProjectionType::Orthographic;

    // 透视相关参数
    float m_perspective_fov = glm::radians(45.0f);  // 垂直视场
    float m_perspective_near = 0.01f;               // 近点
    float m_perspective_far = 1000.0f;              // 远点

    // 正交相关参数
    float m_orthographic_size = 10.0f;
    float m_orthographic_near = -1.0f;
    float m_orthographic_far = 1.0f;

    float m_aspect_ratio = 0.0f;
};

}  // namespace Hazel