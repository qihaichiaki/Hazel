#pragma once

/**
 * imgui layer的实现
 */

#include "Hazel/Core/layer.h"

namespace Hazel
{
class ImGuiLayer : public Layer
{
public:
    HAZEL_API ImGuiLayer();
    HAZEL_API ~ImGuiLayer();

    HAZEL_API void onAttach() override;
    HAZEL_API void onDetach() override;
    HAZEL_API void onImGuiRender() override;
    HAZEL_API void setBlockEvents(bool block_events)
    {
        m_block_events = block_events;
    }
    HAZEL_API void setDarkThemeColors();

    void onEvent(Event&) override;

    /// @brief 初始化imgui帧
    void begin();
    /// @brief 结束imgui帧
    void end();

private:
    bool m_block_events = false;
};

}  // namespace Hazel

struct ImGuiContext;
HAZEL_API inline ImGuiContext* GetImGuiContextPtr();