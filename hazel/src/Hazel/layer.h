#pragma once

/**
 * 层级的抽象类
 */

#include "core.h"
#include "Events/event.h"

namespace Hazel
{

class Layer
{
public:
    HAZEL_API Layer(const std::string& name = "layer");
    HAZEL_API virtual ~Layer();

    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onUpdate() {}
    virtual void onEvent(Event&) {}

    const std::string& getDebugName() const
    {
        return m_debug_name;
    }

private:
    std::string m_debug_name;
};

}  // namespace Hazel