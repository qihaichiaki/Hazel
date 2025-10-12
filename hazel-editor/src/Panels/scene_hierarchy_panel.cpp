#include "scene_hierarchy_panel.h"
#include <imgui.h>
#include <Hazel/Scene/components.h>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{
void SceneHierarchyPanel::onImGuiRenderer()
{
    HZ_ASSERT(m_context, "Scene未被初始化");

    {
        ImGui::Begin("场景层次");
        // 取出当前渲染场景的所有实体对象
        for (auto entity_id : m_context->m_registry.storage<entt::entity>()) {
            Entity entity{entity_id, m_context.get()};
            drawEntityNode(entity);
        }

        // 如果点击当前场景面板的空白区域, 将选择entity置为空
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
            m_selection_context = {};
        }
        ImGui::End();
    }

    {
        ImGui::Begin("对象属性");
        if (m_selection_context) {
            drawComponents(m_selection_context);
        }
        ImGui::End();
    }
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

void SceneHierarchyPanel::drawComponents(Entity entity)
{
    // tag
    if (entity.hasComponent<TagComponent>()) {
        auto& tag = entity.getComponent<TagComponent>().Tag;
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strcpy_s(buffer, sizeof(buffer), tag.c_str());

        if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
            tag = std::string{buffer};
        }
    }

    // transform
    if (entity.hasComponent<TransformComponent>()) {
        if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(),
                              ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
            auto& transform = entity.getComponent<TransformComponent>().Transform;
            ImGui::DragFloat3("position", glm::value_ptr(transform[3]), 0.1f);
            ImGui::TreePop();
        }
    }
}

}  // namespace Hazel