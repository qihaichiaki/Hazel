#pragma once

/**
 * imgui layer的实现
 */

#include "hazel/layer.h"

namespace Hazel
{
class ImGuiLayer : public Layer
{
public:
    HAZEL_API ImGuiLayer();
    HAZEL_API ~ImGuiLayer();

    HAZEL_API void onAttach() override;
    HAZEL_API void onDetach() override;
    HAZEL_API void onUpdate() override;
    HAZEL_API void onEvent(Event&) override;

private:
    float m_time;
};

}  // namespace Hazel