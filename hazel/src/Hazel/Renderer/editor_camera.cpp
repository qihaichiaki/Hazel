#include "hzpch.h"
#include "editor_camera.h"

#include "Hazel/Core/input.h"

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Hazel
{
EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
    : m_FOV(fov),
      m_aspect_ratio(aspectRatio),
      m_near_clip(nearClip),
      m_far_clip(farClip),
      Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
{
    updateView();
}

void EditorCamera::updateProjection()
{
    m_aspect_ratio = m_viewport_width / m_viewport_height;
    m_projection = glm::perspective(glm::radians(m_FOV), m_aspect_ratio, m_near_clip, m_far_clip);
}

void EditorCamera::updateView()
{
    // m_yaw = m_pitch = 0.0f; // Lock the camera's rotation
    m_position = calculatePosition();

    glm::quat orientation = getOrientation();
    m_view_matrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(orientation);
    m_view_matrix = glm::inverse(m_view_matrix);
}

std::pair<float, float> EditorCamera::panSpeed() const
{
    float x = std::min(m_viewport_width / 1000.0f, 2.4f);  // max = 2.4f
    float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    float y = std::min(m_viewport_height / 1000.0f, 2.4f);  // max = 2.4f
    float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    return {xFactor, yFactor};
}

float EditorCamera::rotationSpeed() const
{
    return 0.8f;
}

float EditorCamera::zoomSpeed() const
{
    float distance = m_distance * 0.2f;
    distance = std::max(distance, 0.0f);
    float speed = distance * distance;
    speed = std::min(speed, 100.0f);  // max speed = 100
    return speed;
}

void EditorCamera::onUpdate(Timestep)
{
    if (Input::isKeyPressed(KeyCode::LeftAlt)) {
        const glm::vec2& mouse{Input::getMouseX(), Input::getMouseY()};
        glm::vec2 delta = (mouse - m_initial_mouse_position) * 0.003f;
        m_initial_mouse_position = mouse;

        if (Input::isMouseButtonPressed(KeyCode::MouseButtonMiddle))
            mousePan(delta);
        else if (Input::isMouseButtonPressed(KeyCode::MouseButtonLeft))
            mouseRotate(delta);
        else if (Input::isMouseButtonPressed(KeyCode::MouseButtonRight))
            mouseZoom(delta.y);
    }

    updateView();
}

void EditorCamera::onEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(EditorCamera::onMouseScroll));
}

bool EditorCamera::onMouseScroll(MouseScrolledEvent& e)
{
    float delta = e.getOffsetY() * 0.1f;
    mouseZoom(delta);
    updateView();
    return false;
}

void EditorCamera::mousePan(const glm::vec2& delta)
{
    auto [xSpeed, ySpeed] = panSpeed();
    m_focal_point += -getRightDirection() * delta.x * xSpeed * m_distance;
    m_focal_point += getUpDirection() * delta.y * ySpeed * m_distance;
}

void EditorCamera::mouseRotate(const glm::vec2& delta)
{
    float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
    m_yaw += yawSign * delta.x * rotationSpeed();
    m_pitch += delta.y * rotationSpeed();
}

void EditorCamera::mouseZoom(float delta)
{
    m_distance -= delta * zoomSpeed();
    if (m_distance < 1.0f) {
        m_focal_point += getForwardDirection();
        m_distance = 1.0f;
    }
}

void EditorCamera::setViewportSize(float width, float height)
{
    m_viewport_width = width;
    m_viewport_height = height;
    updateProjection();
}

glm::vec3 EditorCamera::getUpDirection() const
{
    return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 EditorCamera::getRightDirection() const
{
    return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 EditorCamera::getForwardDirection() const
{
    return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 EditorCamera::calculatePosition() const
{
    return m_focal_point - getForwardDirection() * m_distance;
}

glm::quat EditorCamera::getOrientation() const
{
    return glm::quat(glm::vec3(-m_pitch, -m_yaw, 0.0f));
}
}  // namespace Hazel