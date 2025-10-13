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
    if (m_projection_type == ProjectionType::Perspection) {
        m_projection = glm::perspective(m_perspective_fov, m_aspect_ratio, m_perspective_near,
                                        m_perspective_far);
    } else {
        float ortho_left = -0.5f * m_orthographic_size * m_aspect_ratio;
        float ortho_right = 0.5f * m_orthographic_size * m_aspect_ratio;
        float ortho_bottom = -0.5f * m_orthographic_size;
        float ortho_top = 0.5f * m_orthographic_size;

        m_projection = glm::ortho(ortho_left, ortho_right, ortho_bottom, ortho_top,
                                  m_orthographic_near, m_orthographic_far);
    }
}

void SceneCamera::setProjectionType(ProjectionType projection_type)
{
    if (m_projection_type != projection_type) {
        m_projection_type = projection_type;
        recalculateProjection();
    }
}

void SceneCamera::setPerspectionVerticalFOV(float perspective_fov)
{
    m_perspective_fov = perspective_fov;
    recalculateProjection();
}

void SceneCamera::setPerspectiveNearClip(float perspective_near)
{
    m_perspective_near = perspective_near;
    recalculateProjection();
}

void SceneCamera::setPerspectiveFarClip(float perspective_far)
{
    m_perspective_far = perspective_far;
    recalculateProjection();
}

void SceneCamera::setOrthographicSize(float orthographic_size)
{
    m_orthographic_size = orthographic_size;
    recalculateProjection();
}

void SceneCamera::setOrthographicNearClip(float orthographic_near)
{
    m_orthographic_near = orthographic_near;
    recalculateProjection();
}

void SceneCamera::setOrthographicFarClip(float orthographic_far)
{
    m_orthographic_far = orthographic_far;
    recalculateProjection();
}

void SceneCamera::setViewportSize(uint32_t width, uint32_t height)
{
    m_aspect_ratio = (float)width / (float)height;
    recalculateProjection();
}

}  // namespace Hazel