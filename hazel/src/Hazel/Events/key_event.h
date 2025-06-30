#pragma once

/**
 * 键盘事件类型
 */

#include "event.h"

namespace Hazel
{

class HAZEL_API KeyEvent : public Event
{
public:
    int getKeyCode() const
    {
        return m_key_code;
    }

    EVENT_CLASS_CATEGORY(CategoryKeyboard | CategoryInput)

protected:
    KeyEvent(int key_code) : m_key_code{key_code} {}

protected:
    int m_key_code;
};

class HAZEL_API KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent(int key_code, int repeat_count)
        : KeyEvent{key_code}, m_repeat_count{repeat_count}
    {
    }

    EVENT_CLASS_TYPE(KeyPressed)

    std::string toString() const override
    {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << m_key_code << " (" << m_repeat_count << "repeats)";
        return ss.str();
    }

private:
    int m_repeat_count;
};

class HAZEL_API KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent(int key_code) : KeyEvent{key_code} {}

    EVENT_CLASS_TYPE(KeyReleased)

    std::string toString() const override
    {
        std::stringstream ss;
        ss << "KeyReleasedEvent: " << m_key_code;
        return ss.str();
    }
};

class HAZEL_API KeyTypedEvent : public KeyEvent
{
public:
    KeyTypedEvent(int key_code) : KeyEvent{key_code} {}

    EVENT_CLASS_TYPE(KeyTyped)

    std::string toString() const override
    {
        std::stringstream ss;
        ss << "KeyTypedEvent: " << m_key_code;
        return ss.str();
    }
};

}  // namespace Hazel