#pragma once

/**
 * 事件基类, 事件调度器实现
 * 当前事件系统是一个阻塞的, 非延迟的, 实时响应.
 * 未来的扩展方向是可以将事件加入某种事件总线中去
 */

#include "hzpch.h"
#include "Hazel/Core/core.h"

namespace Hazel
{

// 事件类型, 为每个事件对象, 设置为int值
enum class EventType {
    None = 0,
    // 窗口系列
    WindowClose,
    WindowResize,
    WindowFocus,
    WindowLostFocus,
    WindowMoved,
    // 应用程序系列
    AppTick,
    AppUpdate,
    AppRender,
    // 键盘系列
    KeyPressed,
    KeyReleased,
    KeyTyped,
    // 鼠标系列
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled
};

// 事件类别
enum EventCategory {
    None = 0,
    CategoryApplication = BIT(0),
    CategoryInput = BIT(1),
    CategoryKeyboard = BIT(2),
    CategoryMouse = BIT(3),
    CategoryMouseButton = BIT(4)
};

// 下面是一些事件重写宏, 方便子事件实现复用
// 事件类型重写宏
#define EVENT_CLASS_TYPE(type)              \
    static EventType getStaticType()        \
    {                                       \
        return EventType::##type;           \
    }                                       \
    EventType getEventType() const override \
    {                                       \
        return getStaticType();             \
    }                                       \
    const char* getName() const override    \
    {                                       \
        return #type;                       \
    }

// 事件类别重写宏
#define EVENT_CLASS_CATEGORY(category)    \
    int getCategoryFlags() const override \
    {                                     \
        return category;                  \
    }

// 事件基类
class HAZEL_API Event
{
    friend class EventDispatcher;

public:
    virtual ~Event() {}

    /// @brief 返回事件类型
    virtual EventType getEventType() const = 0;
    /// @brief 返回事件名字
    virtual const char* getName() const = 0;
    /// @brief 返回事件类别
    virtual int getCategoryFlags() const = 0;
    /// @brief 返回事件的描述
    virtual std::string toString() const
    {
        return getName();
    }

    /// @brief 当前事件是否属于传入的事件类别
    bool isInCategory(EventCategory event_category)
    {
        return getCategoryFlags() & event_category;
    }

    /// @brief 当前事件是否被处理
    bool isHandled() const
    {
        return m_handled;
    }

protected:
    bool m_handled = false;
};

/// @brief 事件调度器
class EventDispatcher
{
    template <typename T>
    using EventFn = std::function<bool(T&)>;

public:
    EventDispatcher(Event& event) : m_event(event) {}

    /// @brief 事件调度
    /// @tparam EventT 具体事件类型
    /// @param event_fn 事件处理回调
    /// @return 当前传入的回调是否处理
    template <typename EventT, class = std::is_base_of<Event, EventT>>
    bool dispatch(EventFn<EventT> event_fn)
    {
        if (m_event.getEventType() == EventT::getStaticType()) {
            m_event.m_handled = event_fn(*((EventT*)(&m_event)));
            return true;
        }
        return false;
    }

private:
    Event& m_event;
};

inline std::ostream& operator<<(std::ostream& os, Event& event)
{
    return os << event.toString();
}

}  // namespace Hazel