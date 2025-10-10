#pragma once

/**
 * 运行时相机, 充当相机组件的数据类型
 */

#include <glm/glm.hpp>

namespace Hazel
{

class Camera
{
public:
    HAZEL_API Camera() = default;
    HAZEL_API Camera(const glm::mat4& projection) : m_projection{projection} {}
    HAZEL_API virtual ~Camera() = default;

    /// @brief 返回相机的投影矩阵
    HAZEL_API const glm::mat4& getProjection() const
    {
        return m_projection;
    }

protected:
    glm::mat4 m_projection{1.0f};  // 初始化为单位矩阵
};
}  // namespace Hazel