#include "content_browser_panel.h"

#include <imgui_internal.h>
#include <imgui.h>

namespace Hazel
{

static const std::filesystem::path s_assert_path = "./assets";

ContentBrowserPanel::ContentBrowserPanel()
{
    m_current_file_path = s_assert_path;
}
ContentBrowserPanel::~ContentBrowserPanel() {}

void ContentBrowserPanel::onImGuiRenderer()
{
    ImGui::Begin("内容面板");

    ImGui::BeginDisabled(m_current_file_path == s_assert_path);
    if (ImGui::Button("<_  ")) {
        m_current_file_path = m_current_file_path.parent_path();
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::Text("%s",
                ("assets/" + m_current_file_path.lexically_relative(s_assert_path).generic_string())
                    .c_str());
    for (auto it : std::filesystem::directory_iterator{m_current_file_path}) {
        const auto& path = it.path();
        std::string file_name = path.filename().string();

        std::string path_str = path.generic_string();
        ImGui::PushID(path_str.c_str());
        ImGui::Button(file_name.c_str());
        bool is_directory = it.is_directory();
        if (!is_directory && ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("CONTEXT_ITEM", path_str.c_str(),
                                      (path_str.size() + 1) * sizeof(char), ImGuiCond_Once);
            ImGui::EndDragDropSource();
        }

        if (is_directory && ImGui::IsItemHovered() &&
            ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            m_current_file_path /= file_name;
        }

        ImGui::PopID();
    }

    ImGui::End();
}

}  // namespace Hazel