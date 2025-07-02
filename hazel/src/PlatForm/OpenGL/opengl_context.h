#pragma once

#include "Hazel/Renderer/graphics_context.h"

struct GLFWwindow;
namespace Hazel
{

class OpenGlContext : public GraphicsContext
{
public:
    OpenGlContext(GLFWwindow* window_handle);
    ~OpenGlContext();

    void init() override;
    void swapBuffer() override;

private:
    GLFWwindow* m_window_handle;
};
}  // namespace Hazel