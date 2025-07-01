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
    HAZEL_API void onImGuiRender() override;

    /// @brief 初始化imgui帧
    void begin();
    /// @brief 结束imgui帧
    void end();
};

}  // namespace Hazel