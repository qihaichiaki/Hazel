#include "hzpch.h"
#include "renderer_command.h"

#include "PlatForm/OpenGL/opengl_renderer_api.h"

namespace Hazel
{
// 临时初始化, 后续根据具体的后端配置选择
// 不需要析构, 因为是和应用程序声明周期同步
Scope<RendererAPI> RendererCommand::s_renderer_api = std::make_unique<OpenGLRendererAPI>();
}