#include "scene_hierarchy_panel.h"
#include <imgui.h>
#include <Hazel/Scene/components.h>

namespace Hazel
{
void SceneHierarchyPanel::onImGuiRenderer()
{
    HZ_ASSERT(m_context, "Scene未被初始化");

    ImGui::Begin("场景层次");

    // 取出当前渲染场景的所有实体对象
    for (auto entity_id : m_context->m_registry.storage<entt::entity>()) {
        Entity entity{entity_id, m_context.get()};
        drawEntityNode(entity);
    }

    ImGui::End();
}

void SceneHierarchyPanel::drawEntityNode(Entity entity)
{
    auto& tag = entity.getComponent<TagComponent>().Tag;
    // ImGuiTreeNodeFlags_OpenOnArrow 是存在点击展开
    ImGuiTreeNodeFlags flags = (m_selection_context == entity ? ImGuiTreeNodeFlags_Selected : 0) |
                               ImGuiTreeNodeFlags_OpenOnArrow;
    bool opened = ImGui::TreeNodeEx((void*)(uintptr_t)(uint32_t)entity, flags, tag.c_str());
    // 成功点击
    if (ImGui::IsItemClicked()) {
        m_selection_context = entity;
    }

    if (opened) {
        // 模拟子元素
        ImGuiTreeNodeFlags _flags = ImGuiTreeNodeFlags_OpenOnArrow;
        bool _opened =
            ImGui::TreeNodeEx((void*)(uintptr_t)((uint32_t)entity + 100), _flags, "测试子对象");
        if (_opened) ImGui::TreePop();
        ImGui::TreePop();
    }
}

}  // namespace Hazel