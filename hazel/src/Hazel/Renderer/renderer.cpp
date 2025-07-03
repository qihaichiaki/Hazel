#include "hzpch.h"
#include "renderer.h"

namespace Hazel
{
// 暂时直接在本处初始化
RendererAPI Renderer::s_renderer_api = RendererAPI::OpenGL;

}  // namespace Hazel