#pragma once

/**
 * 编辑器相机
 */

#include <Hazel/Renderer/camera.h>
#include <Hazel/Core/timestep.h>
#include <Hazel/Events/mouse_event.h>

#include <glm/glm.hpp>

namespace Hazel
{

class EditorCamera : public Camera
{
public:
    HAZEL_API EditorCamera() = default;
    HAZEL_API EditorCamera(float fov, float aspect_ratio, float near_clip, float far_clip);

    HAZEL_API void onUpdate(Timestep ts);
    HAZEL_API void onEvent(Event& e);

    HAZEL_API float getDistance() const
    {
        return m_distance;
    }
    HAZEL_API void setDistance(float distance)
    {
        m_distance = distance;
    }

    HAZEL_API void setViewportSize(float width, float height);

    HAZEL_API const glm::mat4& getViewMatrix() const
    {
        return m_view_matrix;
    }
    HAZEL_API glm::mat4 getViewProjection() const
    {
        return m_projection * m_view_matrix;
    }

    HAZEL_API glm::vec3 getUpDirection() const;
    HAZEL_API glm::vec3 getRightDirection() const;
    HAZEL_API glm::vec3 getForwardDirection() const;
    HAZEL_API const glm::vec3& getPosition() const
    {
        return m_position;
    }
    HAZEL_API glm::quat getOrientation() const;

    HAZEL_API float getPitch() const
    {
        return m_pitch;
    }
    HAZEL_API float getYaw() const
    {
        return m_yaw;
    }

private:
    void updateProjection();
    void updateView();

    bool onMouseScroll(MouseScrolledEvent& e);

    void mousePan(const glm::vec2& delta);
    void mouseRotate(const glm::vec2& delta);
    void mouseZoom(float delta);

    glm::vec3 calculatePosition() const;

    std::pair<float, float> panSpeed() const;
    float rotationSpeed() const;
    float zoomSpeed() const;

private:
    float m_FOV = 45.0f, m_aspect_ratio = 1.778f, m_near_clip = 0.1f, m_far_clip = 1000.0f;

    glm::mat4 m_view_matrix;
    glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
    glm::vec3 m_focal_point = {0.0f, 0.0f, 0.0f};

    glm::vec2 m_initial_mouse_position = {0.0f, 0.0f};

    float m_distance = 10.0f;
    float m_pitch = 0.0f, m_yaw = 0.0f;

    float m_viewport_width = 1280, m_viewport_height = 720;
};
}  // namespace Hazel