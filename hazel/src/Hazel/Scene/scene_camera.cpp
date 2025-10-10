#include "hzpch.h"
#include "scene_camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel
{
SceneCamera::SceneCamera()
{
    recalculateProjection();
}

SceneCamera::~SceneCamera() {}

void SceneCamera::recalculateProjection()
{
    float ortho_left = -0.5f * m_orthographic_size * m_aspect_ratio;
    float ortho_right = 0.5f * m_orthographic_size * m_aspect_ratio;
    float ortho_bottom = -0.5f * m_orthographic_size;
    float ortho_top = 0.5f * m_orthographic_size;

    m_projection = glm::ortho(ortho_left, ortho_right, ortho_bottom, ortho_top, m_orthographic_near,
                              m_orthographic_far);
}

void SceneCamera::setOrthographic(float orthographic_size, float z_near, float z_far)
{
    m_orthographic_size = orthographic_size;
    m_orthographic_near = z_near;
    m_orthographic_far = z_far;

    recalculateProjection();
}

void SceneCamera::setOrthographicSize(float orthographic_size)
{
    m_orthographic_size = orthographic_size;
    recalculateProjection();
}

void SceneCamera::setViewportSize(uint32_t width, uint32_t height)
{
    m_aspect_ratio = (float)width / (float)height;
    recalculateProjection();
}

}  // namespace Hazel