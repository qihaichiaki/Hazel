#include "hzpch.h"
#include "imgui_layer.h"

#include "Hazel/Core/application.h"

#include "imgui.h"
#include "GLFW/glfw3.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace Hazel
{

ImGuiLayer::ImGuiLayer() : Layer{"ImGui"} {}
ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::onAttach()
{
    HZ_PROFILE_FUNCTION();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();  // 风格设置

    // ImGui后端IO设置
    auto& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/msyh.ttc", 18.0f, nullptr,
                                 io.Fonts->GetGlyphRangesChineseFull());
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    auto& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
    // glfw后端初始化
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    // opengl渲染后端的初始化
    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::onDetach()
{
    HZ_PROFILE_FUNCTION();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::onImGuiRender()
{
    HZ_PROFILE_FUNCTION();

    static bool is_show_demo = true;
    ImGui::ShowDemoWindow(&is_show_demo);
}

void ImGuiLayer::begin()
{
    HZ_PROFILE_FUNCTION();

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::end()
{
    HZ_PROFILE_FUNCTION();

    // 更新窗口大小
    auto& io = ImGui::GetIO();
    auto& window = Application::get().getWindow();
    io.DisplaySize = ImVec2{(float)window.getWidth(), (float)window.getHeight()};

    // imgui提交渲染任务
    ImGui::Render();
    // 后端实际使用opengl执行绘制任务
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

}  // namespace Hazel