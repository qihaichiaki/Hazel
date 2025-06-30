#pragma once

/**
 * 应用程序事件类型
 * Window + App
 */

#include "event.h"

namespace Hazel
{

class HAZEL_API WindowCloseEvent : public Event
{
public:
    EVENT_CLASS_TYPE(WindowClose)
    EVENT_CLASS_CATEGORY(CategoryApplication)
};

class HAZEL_API WindowResizeEvent : public Event
{
public:
    WindowResizeEvent(unsigned int width, unsigned int height) : m_width{width}, m_height{height} {}

    unsigned int getResizeWidth() const
    {
        return m_width;
    }

    unsigned int getResizeHeight() const
    {
        return m_height;
    }

    EVENT_CLASS_TYPE(WindowResize)
    EVENT_CLASS_CATEGORY(CategoryApplication)

    std::string toString() const override
    {
        std::stringstream ss;
        ss << "WindowResizeEvent: (" << m_width << ", " << m_height << ")";
        return ss.str();
    }

private:
    unsigned int m_width, m_height;
};

class HAZEL_API AppTickEvent : public Event
{
public:
    EVENT_CLASS_TYPE(AppTick)
    EVENT_CLASS_CATEGORY(CategoryApplication)
};

class HAZEL_API AppUpdateEvent : public Event
{
public:
    EVENT_CLASS_TYPE(AppUpdate)
    EVENT_CLASS_CATEGORY(CategoryApplication)
};

class HAZEL_API AppRenderEvent : public Event
{
public:
    EVENT_CLASS_TYPE(AppRender)
    EVENT_CLASS_CATEGORY(CategoryApplication)
};

}  // namespace Hazel