#include "hzpch.h"
#include "ortho_graphic_camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel
{

OrthoGraphicCamera::OrthoGraphicCamera(float left, float right, float bottom, float top)
    : m_view_matrix{1.0f},                                                     // 单位矩阵
      m_projection_matrix{glm::ortho(left, right, bottom, top, -1.0f, 1.0f)},  // 初始化投影矩阵
      m_position{0.0f},
      m_rotation{0.0f}
{
    m_projection_view_matrix = m_projection_matrix * m_view_matrix;
}

OrthoGraphicCamera::~OrthoGraphicCamera() {}

void OrthoGraphicCamera::recalculateViewMartix()
{
    // 先计算transform矩阵, 然后执行逆矩阵即可
    glm::mat4 transform =
        glm::translate(glm::mat4{1.0f}, m_position) *
        glm::rotate(glm::mat4{1.0f}, glm::radians(m_rotation), glm::vec3{0.0f, 0.0f, 1.0f});
    m_view_matrix = glm::inverse(transform);

    m_projection_view_matrix = m_projection_matrix * m_view_matrix;
}

}  // namespace Hazel