#pragma once

/**
 * 层级的抽象类
 */

#include "core.h"
#include "Hazel/Events/event.h"
#include "Hazel/Core/timestep.h"

namespace Hazel
{

class Layer
{
public:
    HAZEL_API Layer(const std::string& name = "layer");
    HAZEL_API virtual ~Layer();

    /// @brief layer被push时调用的加载方法
    HAZEL_API virtual void onAttach() {}

    /// @brief layer被pop时调用的卸载方法
    HAZEL_API virtual void onDetach() {}

    /// @brief layer的更新动作
    HAZEL_API virtual void onUpdate(Timestep) {}

    /// @brief layer的事件处理
    HAZEL_API virtual void onEvent(Event&) {}

    /// @brief layer的imgui更新处理
    HAZEL_API virtual void onImGuiRender() {}

    /// @brief 返回layer层的debug name
    HAZEL_API const std::string& getDebugName() const
    {
        return m_debug_name;
    }

private:
    std::string m_debug_name;
};

}  // namespace Hazel