#pragma once

/**
 * 鼠标事件类型
 */

#include "event.h"

namespace Hazel
{

class HAZEL_API MouseButtonEvent : public Event
{
public:
    int getMouseButton() const
    {
        return m_button;
    }

    EVENT_CLASS_CATEGORY(CategoryMouse | CategoryMouseButton | CategoryInput)

protected:
    MouseButtonEvent(int button) : m_button{button} {}

protected:
    int m_button;
};

class HAZEL_API MouseButtonPressedEvent : public MouseButtonEvent
{
public:
    MouseButtonPressedEvent(int button) : MouseButtonEvent{button} {}

    EVENT_CLASS_TYPE(MouseButtonPressed)

    std::string toString() const override
    {
        std::stringstream ss;
        ss << "MouseButtonPressedEvent: " << m_button;
        return ss.str();
    }
};

class HAZEL_API MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
    MouseButtonReleasedEvent(int button) : MouseButtonEvent{button} {}

    EVENT_CLASS_TYPE(MouseButtonReleased)

    std::string toString() const override
    {
        std::stringstream ss;
        ss << "MouseButtonReleasedEvent: " << m_button;
        return ss.str();
    }
};

class HAZEL_API MouseMovedEvent : public Event
{
public:
    MouseMovedEvent(float x_pos, float y_pos) : m_x_pos{x_pos}, m_y_pos{y_pos} {}

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput)

    std::string toString() const override
    {
        std::stringstream ss;
        ss << "MouseMovedEvent: (" << m_x_pos << ", " << m_y_pos << ")";
        return ss.str();
    }

private:
    float m_x_pos, m_y_pos;
};

class HAZEL_API MouseScrolledEvent : public Event
{
public:
    MouseScrolledEvent(float x_offset, float y_offset) : m_x_offset{x_offset}, m_y_offset{y_offset}
    {
    }

    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(CategoryMouse | CategoryInput)

    std::string toString() const override
    {
        std::stringstream ss;
        ss << "MouseScrolledEvent: (" << m_x_offset << ", " << m_y_offset << ")";
        return ss.str();
    }

private:
    float m_x_offset, m_y_offset;
};

}  // namespace Hazel