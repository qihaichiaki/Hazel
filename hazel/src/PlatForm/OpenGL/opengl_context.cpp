#include "hzpch.h"

#include "opengl_context.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Hazel
{
OpenGlContext::OpenGlContext(GLFWwindow* window_handle) : m_window_handle{window_handle}
{
    HZ_CORE_ASSERT(m_window_handle, "GlFWwindow不可为空!");
}
OpenGlContext::~OpenGlContext() {}

void OpenGlContext::init()
{
    HZ_PROFILE_FUNCTION();

    // glfw创建当前的上下文
    glfwMakeContextCurrent(m_window_handle);
    // glad加载所有现代opengl函数
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    HZ_CORE_ASSERT(status, "gladLoadGLLoader 失败!");

    // 输出当前上下文使用gpu信息:
    HZ_CORE_INFO("当前OpenGLContext使用GPU信息: ");
    HZ_CORE_INFO("[+=======================================================");
    HZ_CORE_INFO("|+ vendor:   {}", (char*)(glGetString(GL_VENDOR)));
    HZ_CORE_INFO("|+ renderer: {}", (char*)(glGetString(GL_RENDERER)));
    HZ_CORE_INFO("|+ version:  {}", (char*)(glGetString(GL_VERSION)));
    HZ_CORE_INFO("[+=======================================================");
}

void OpenGlContext::swapBuffer()
{
    HZ_PROFILE_FUNCTION();

    glfwSwapBuffers(m_window_handle);  // 此处如果设置了垂直同步, 那么会根据屏幕刷新hz进行等待......
}

}  // namespace Hazel