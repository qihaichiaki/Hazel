#include "hzpch.h"
#include "opengl_renderer_api.h"

#include <glad/glad.h>

namespace Hazel
{
void OpenGLRendererAPI::init()
{
    HZ_PROFILE_FUNCTION();

    // 启用纹理混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // 经典混合函数, 实现透明效果
    // Final = Src.RGB * sfactor + Dst.RGB * dfactor
    // sfactor = Src.A
    // dfactor = (1 - src.A)

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
}

void OpenGLRendererAPI::setClearColor(const glm::vec4& clear_color)
{
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
}

void OpenGLRendererAPI::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::drawIndexed(const Ref<VertexArray>& vertex_array, uint32_t index_count)
{
    index_count = index_count == 0 ? vertex_array->getIndexBuffer()->getCount() : index_count;
    glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);  // 清空绑定纹理槽
}

void OpenGLRendererAPI::setViewport(uint32_t width, uint32_t height)
{
    glViewport(0, 0, width, height);
}

}  // namespace Hazel