#include "hzpch.h"
#include "imgui_layer.h"

#include "PlatForm/OpenGL/imgui_impl_opengl3.h"
#include "Hazel/application.h"

#include "GLFW/glfw3.h"

namespace Hazel
{

ImGuiLayer::ImGuiLayer() : Layer{"ImGui"}, m_time{0.f} {}
ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::onAttach()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();  // 风格设置

    // ImGui后端IO设置
    auto& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;  // 后端支持imgui根据当前状态切换鼠标光标
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;   // 后端支持程序性的设置鼠标位置

    // imgui按键映射 todo: 和教程不一致, 新版imgui使用AddXXXEvent的形式实现映射,
    // 抛弃了KeyMap和KeyDown的相关写法

    // opengl渲染后端的初始化
    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::onDetach() {}

void ImGuiLayer::onUpdate()
{
    // 更新窗口大小
    auto& io = ImGui::GetIO();
    auto& window = Application::get().getWindow();
    io.DisplaySize = ImVec2{(float)window.getWidth(), (float)window.getHeight()};

    // 设置帧间隔
    float current_time = (float)glfwGetTime();
    io.DeltaTime = m_time > 0.0 ? (float)(current_time - m_time) : (float)(1.0f / 60.0f);
    m_time = current_time;

    // opengl 新帧更新
    ImGui_ImplOpenGL3_NewFrame();
    // imgui开启新帧
    ImGui::NewFrame();

    // 显示demo窗口
    static bool is_show = true;
    ImGui::ShowDemoWindow(&is_show);

    // imgui提交渲染任务
    ImGui::Render();
    // 后端实际使用opengl执行绘制任务
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::onEvent(Event&) {}

}  // namespace Hazel