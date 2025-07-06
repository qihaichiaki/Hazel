#pragma once

/**
 * 正交2d相机
 * 正交的投影矩阵(如何将观察范围映射到屏幕上), 基于相机自身的视图矩阵
 * vp(基于opengl数学库, 就是列向量, 所以实际应该是pv)
 */

#include <glm/glm.hpp>

namespace Hazel
{

/// @brief 2d正交相机
class OrthoGraphicCamera
{
public:
    OrthoGraphicCamera(float left, float right, float bottom, float top);
    ~OrthoGraphicCamera();

    /// @brief 返回相机的世界坐标
    const glm::vec3& getPosition() const
    {
        return m_position;
    }
    /// @brief 设置相机自身的世界坐标
    void setPosition(const glm::vec3& position)
    {
        m_position = position;
        recalculateViewMartix();
    }
    /// @brief 返回相机当前基于z轴的逆时针旋转角度
    float getRotation() const
    {
        return m_rotation;
    }
    /// @brief 设置2d相机基于z轴逆时针的旋转
    /// @param rotation 旋转角度
    void setRotation(float rotation)
    {
        m_rotation = rotation;
        recalculateViewMartix();
    }

    /// @brief 返回基于相机transform的视图矩阵
    const glm::mat4& getViewMatrix() const
    {
        return m_view_matrix;
    }
    /// @brief 返回正交相机的正交投影矩阵
    const glm::mat4& getProjectionMatrix() const
    {
        return m_projection_matrix;
    }
    /// @brief 返回p x v 矩阵缓存(opengl列向量视角)
    const glm::mat4& getProjectionViewMatrix() const
    {
        return m_projection_view_matrix;
    }

private:
    /// @brief 发生transform后需要重新计算view矩阵
    void recalculateViewMartix();

private:
    glm::mat4 m_view_matrix;        // 视图矩阵, 基于相机的transform
    glm::mat4 m_projection_matrix;  // 正交投影矩阵(类似于拍照时的`咔擦`), 当前支持opengl右手坐标系
    glm::mat4 m_projection_view_matrix;  // pv 矩阵缓存, 方便直接上传给gpu使用

    glm::vec3 m_position;  // 相机的世界坐标
    float m_rotation;      // 相机旋转角度, 基于z轴, 逆时针
};

}  // namespace Hazel