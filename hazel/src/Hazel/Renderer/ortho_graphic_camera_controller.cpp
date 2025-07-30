#include "hzpch.h"
#include "ortho_graphic_camera_controller.h"

#include "Hazel/Core/input.h"
#include "Hazel/Core/input_codes.h"

namespace Hazel
{
OrthoGraphicCameraController::OrthoGraphicCameraController(float aspect_ratio)
    : m_aspect_ratio{aspect_ratio},
      m_camera{-aspect_ratio * m_zoom_level, aspect_ratio * m_zoom_level, -m_zoom_level,
               m_zoom_level}
{
}

OrthoGraphicCameraController::~OrthoGraphicCameraController() {}

void OrthoGraphicCameraController::onUpdate(Timestep ts)
{
    HZ_PROFILE_FUNCTION();

    // 简单使用input系统实现相机移动
    if (Hazel::Input::isKeyPressed(HZ_KEY_A)) {
        m_camera_position.x -=
            std::cos(glm::radians(m_camera_rotation)) * m_camera_translation_speed * ts;
        m_camera_position.y -=
            std::sin(glm::radians(m_camera_rotation)) * m_camera_translation_speed * ts;
    } else if (Hazel::Input::isKeyPressed(HZ_KEY_D)) {
        m_camera_position.x +=
            std::cos(glm::radians(m_camera_rotation)) * m_camera_translation_speed * ts;
        m_camera_position.y +=
            std::sin(glm::radians(m_camera_rotation)) * m_camera_translation_speed * ts;
    }
    if (Hazel::Input::isKeyPressed(HZ_KEY_W)) {
        m_camera_position.x +=
            -std::sin(glm::radians(m_camera_rotation)) * m_camera_translation_speed * ts;
        m_camera_position.y +=
            std::cos(glm::radians(m_camera_rotation)) * m_camera_translation_speed * ts;
    } else if (Hazel::Input::isKeyPressed(HZ_KEY_S)) {
        m_camera_position.x -=
            -std::sin(glm::radians(m_camera_rotation)) * m_camera_translation_speed * ts;
        m_camera_position.y -=
            std::cos(glm::radians(m_camera_rotation)) * m_camera_translation_speed * ts;
    }
    // 旋转
    if (Hazel::Input::isKeyPressed(HZ_KEY_Q)) {
        m_camera_rotation += m_camera_rotation_speed * ts;
    } else if (Hazel::Input::isKeyPressed(HZ_KEY_E)) {
        m_camera_rotation -= m_camera_rotation_speed * ts;
    }

    if (m_camera_rotation > 180.0f) {
        m_camera_rotation -= 360.0f;
    } else if (m_camera_rotation < -180.0f) {
        m_camera_rotation += 360.0f;
    }

    m_camera.setPosition(m_camera_position);
    m_camera.setRotation(m_camera_rotation);
}

void OrthoGraphicCameraController::onEvent(Event& e)
{
    HZ_PROFILE_FUNCTION();

    EventDispatcher event_dispatcher{e};

    event_dispatcher.dispatch<MouseScrolledEvent>(
        HZ_BIND_EVENT_FN(OrthoGraphicCameraController::onMouseScrolled));
    event_dispatcher.dispatch<WindowResizeEvent>(
        HZ_BIND_EVENT_FN(OrthoGraphicCameraController::onWindowResized));
}

bool OrthoGraphicCameraController::onMouseScrolled(MouseScrolledEvent& e)
{
    HZ_PROFILE_FUNCTION();

    m_zoom_level -= e.getOffsetY() * 0.25f;
    m_zoom_level = std::max(m_zoom_level, 0.25f);
    m_camera_translation_speed = m_zoom_level;
    m_camera.setProjection(-m_aspect_ratio * m_zoom_level, m_aspect_ratio * m_zoom_level,
                           -m_zoom_level, m_zoom_level);
    return false;
}
bool OrthoGraphicCameraController::onWindowResized(WindowResizeEvent& e)
{
    HZ_PROFILE_FUNCTION();

    m_aspect_ratio = 1.0f * e.getResizeWidth() / e.getResizeHeight();
    m_camera.setProjection(-m_aspect_ratio * m_zoom_level, m_aspect_ratio * m_zoom_level,
                           -m_zoom_level, m_zoom_level);
    return false;
}

}  // namespace Hazel