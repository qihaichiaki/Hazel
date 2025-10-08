#pragma once

/**
 * 正交相机控制器
 */

#include "ortho_graphic_camera.h"
#include "Hazel/Events/application_event.h"
#include "Hazel/Events/mouse_event.h"
#include "Hazel/Core/timestep.h"

namespace Hazel
{

struct OrthoGraphicCameraBounds
{
    float Left, Right;
    float Bottom, Top;

    float getWidth() const
    {
        return Right - Left;
    }
    float getHeight() const
    {
        return Top - Bottom;
    };
};

class OrthoGraphicCameraController
{
public:
    HAZEL_API OrthoGraphicCameraController(float aspect_ratio);
    HAZEL_API ~OrthoGraphicCameraController();

    HAZEL_API void onUpdate(Timestep ts);
    HAZEL_API void onEvent(Event&);

    /// @brief 调整大小时应该调用的
    HAZEL_API void onResize(float width, float height);

    HAZEL_API const OrthoGraphicCamera& getCamera() const
    {
        return m_camera;
    }

    HAZEL_API const OrthoGraphicCameraBounds& getBounds() const
    {
        return m_bounds;
    }

private:
    bool onMouseScrolled(MouseScrolledEvent&);
    bool onWindowResized(WindowResizeEvent&);

private:
    float m_aspect_ratio;       // 宽高比
    float m_zoom_level = 1.0f;  // 缩放级别
    OrthoGraphicCameraBounds m_bounds;
    OrthoGraphicCamera m_camera;

    glm::vec3 m_camera_position{0.0f};        // 相机的世界坐标
    float m_camera_rotation = 0.0f;           // 相机旋转角度
    float m_camera_translation_speed = 1.0f;  // 相机的移动速度
    float m_camera_rotation_speed = 180.0f;   // 相机的旋转速度
};
}  // namespace Hazel