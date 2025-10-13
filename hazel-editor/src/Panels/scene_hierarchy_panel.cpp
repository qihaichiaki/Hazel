#include "scene_hierarchy_panel.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <Hazel/Scene/components.h>
#include <Hazel/Core/log.h>
#include <Hazel/ImGui/imgui_layer.h>

namespace Hazel
{
void SceneHierarchyPanel::onImGuiRenderer()
{
    HZ_ASSERT(m_context, "Scene未被初始化");

    {
        ImGui::Begin("场景层次");
        // 取出当前渲染场景的所有实体对象
        m_context->m_registry.view<entt::entity>().each([this](auto entity_id) {
            Entity entity{entity_id, m_context.get()};
            drawEntityNode(entity);
        });

        // 如果点击当前场景面板的空白区域, 将选择entity置为空
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
            m_selection_context = {};
        }

        // 如果在场景层次右键空白区域, 弹出当前窗口的上下文窗口
        // flag: 右键打开 | 悬停在项目上时不返回true，仅在悬停在空白处时返回。
        if (ImGui::BeginPopupContextWindow(
                0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
            if (ImGui::MenuItem("创建空实体")) {
                m_context->createEntity("空实体");
            }
            ImGui::EndPopup();
        }
        ImGui::End();
    }

    {
        ImGui::Begin("对象属性");
        if (m_selection_context) {
            drawComponents(m_selection_context);

            {
                // 居中按钮
                // 获取窗口内容区域宽度
                float window_width =
                    ImGui::GetContentRegionAvail().x + ImGui::GetStyle().FramePadding.x * 2;
                // 下一个按钮的宽度
                float button_width = 100.0f;
                // 计算居中位置
                float offsetX = (window_width - button_width) * 0.5f;
                if (offsetX > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
            }
            if (ImGui::Button("添加组件")) {
                // 添加组件打开对应id的上下文窗口
                ImGui::OpenPopup("AddComponent");
            }

            if (ImGui::BeginPopup("AddComponent")) {
                if (ImGui::MenuItem("Camera")) {
                    m_selection_context.addComponent<CameraComponent>();
                    // ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("SpriteRenderer")) {
                    m_selection_context.addComponent<SpriteRendererComponent>();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }
}

// 场景层次绘制树状节点
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

    // 右键场景层次节点, 存在相关辅助操作, 比如删除节点
    bool is_deleted_entity = false;
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("删除实体")) {
            is_deleted_entity = true;  // 标记删除
        }
        ImGui::EndPopup();
    }

    if (opened) {
        // 模拟子元素
        ImGuiTreeNodeFlags _flags = ImGuiTreeNodeFlags_OpenOnArrow;
        bool _opened =
            ImGui::TreeNodeEx((void*)(uintptr_t)((uint32_t)entity + 100), _flags, "测试子对象");
        if (_opened) ImGui::TreePop();
        ImGui::TreePop();
    }

    if (is_deleted_entity) {
        m_context->destoryEntity(entity);
        if (m_selection_context == entity) {
            m_selection_context = {};  // 此处也需要置空, 防止属性面板访问空的实体对象
        }
    }
}

// TODO: 后续会转移到自定义UI库中
static bool drawVec3Contol(const std::string& label,
                           glm::vec3& values,
                           float reset_value = 0.0f,
                           float column_width = 100.0f)
{
    bool is_col = false;

    ImGui::PushID(label.c_str());
    ImGui::Columns(2);                       // 开始设置为2列
    ImGui::SetColumnWidth(0, column_width);  // 设置第一列的宽度
    ImGui::Text(label.c_str());
    ImGui::NextColumn();                                     // 开始添加下一行
    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());  // 设置多个item的宽度
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.0f, 0.0f});
    auto gimgui = GetImGuiContextPtr();
    float line_height = gimgui->FontSize + gimgui->Style.FramePadding.y * 2.0f;  // 计算行高
    ImVec2 button_size{line_height + 3.0f, line_height};
    // 创建按钮 x, y, z
    // 按钮颜色
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    if (ImGui::Button("X", button_size)) {
        values.x = reset_value;
        is_col = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    if (ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f")) {
        is_col = true;
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    if (ImGui::Button("Y", button_size)) {
        values.y = reset_value;
        is_col = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    if (ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f")) {
        is_col = true;
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    if (ImGui::Button("Z", button_size)) {
        values.z = reset_value;
        is_col = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    if (ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f")) {
        is_col = true;
    }
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();
    ImGui::Columns(1);
    ImGui::PopID();

    return is_col;
}

// 属性面板绘制组件信息
void SceneHierarchyPanel::drawComponents(Entity entity)
{
    // tag
    if (entity.hasComponent<TagComponent>()) {
        auto& tag = entity.getComponent<TagComponent>().Tag;
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strcpy_s(buffer, sizeof(buffer), tag.c_str());

        // "##Tag" 重名渲染, 会导致不同对象设置点击一致的效果
        if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
            tag = std::string{buffer};
        }
        ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y * 2.0f));  // 空两倍默认间距
    }

    // 默认展开 | 运行项目重叠（目的时在treenode这一行继续添加控件）
    const ImGuiTreeNodeFlags tree_node_flags =
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;
    // transform
    if (entity.hasComponent<TransformComponent>()) {
        bool is_open = ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(),
                                         tree_node_flags, "Transform");
        if (is_open) {
            auto& transform_component = entity.getComponent<TransformComponent>();
            drawVec3Contol("位置", transform_component.Translation);
            glm::vec3 d_roation = glm::degrees(transform_component.Rotation);
            if (drawVec3Contol("旋转", d_roation)) {
                transform_component.Rotation = glm::radians(d_roation);
            }
            drawVec3Contol("缩放", transform_component.Scale, 1.0f);
            ImGui::TreePop();
            ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y * 2.0f));  // 空两倍默认间距
        }
    }

    // Camera
    if (entity.hasComponent<CameraComponent>()) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
        bool is_open = ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(),
                                         tree_node_flags, "Camera");
        ImGui::SameLine(ImGui::GetWindowWidth() - 32.0f);
        if (ImGui::Button("•••"))  // 扩展按钮
        {
            ImGui::OpenPopup("ComponentSettings");
        }
        ImGui::PopStyleVar();

        bool remove_component = false;
        if (ImGui::BeginPopup("ComponentSettings")) {
            if (ImGui::MenuItem("删除组件")) {
                remove_component = true;
            }
            ImGui::EndPopup();
        }

        if (is_open) {
            auto& camera_component = entity.getComponent<CameraComponent>();

            ImGui::Checkbox("渲染", &camera_component.Primary);
            const char* projection_type_strings[] = {"透视", "正交"};
            const char* current_projection_type_string =
                projection_type_strings[(int)camera_component.Camera.getProjectionType()];
            if (ImGui::BeginCombo("投影类型", current_projection_type_string)) {
                for (int i = 0; i < 2; ++i) {
                    bool is_selected = current_projection_type_string == projection_type_strings[i];
                    if (ImGui::Selectable(projection_type_strings[i], is_selected)) {
                        current_projection_type_string = projection_type_strings[i];
                        camera_component.Camera.setProjectionType((SceneCamera::ProjectionType)i);
                    }

                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (strcmp(current_projection_type_string, "透视") == 0) {
                float fov_degree =
                    glm::degrees(camera_component.Camera.getPerspectionVerticalFOV());
                if (ImGui::DragFloat("垂直视野", &fov_degree)) {
                    camera_component.Camera.setPerspectionVerticalFOV(glm::radians(fov_degree));
                }
                float p_near = camera_component.Camera.getPerspectiveNearClip();
                if (ImGui::DragFloat("近点", &p_near)) {
                    camera_component.Camera.setPerspectiveNearClip(p_near);
                }
                float p_far = camera_component.Camera.getPerspectiveFarClip();
                if (ImGui::DragFloat("远点", &p_far)) {
                    camera_component.Camera.setPerspectiveFarClip(p_far);
                }
            } else {
                float o_size = camera_component.Camera.getOrthographicSize();
                if (ImGui::DragFloat("正交大小", &o_size)) {
                    camera_component.Camera.setOrthographicSize(o_size);
                }
                float o_near = camera_component.Camera.getOrthographicNearClip();
                if (ImGui::DragFloat("近点", &o_near)) {
                    camera_component.Camera.setOrthographicNearClip(o_near);
                }
                float o_far = camera_component.Camera.getOrthographicFarClip();
                if (ImGui::DragFloat("远点", &o_far)) {
                    camera_component.Camera.setOrthographicFarClip(o_far);
                }
            }

            ImGui::Checkbox("固定纵横比", &camera_component.FixedAspectRatio);
            ImGui::TreePop();
            ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y * 2.0f));  // 空两倍默认间距
        }

        if (remove_component) {
            entity.removeComponent<CameraComponent>();
        }
    }

    // SpriteRenderer
    if (entity.hasComponent<SpriteRendererComponent>()) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
        bool is_open = ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(),
                                         tree_node_flags, "Sprite");
        ImGui::SameLine(ImGui::GetWindowWidth() - 32.0f);
        if (ImGui::Button("•••"))  // 扩展按钮
        {
            ImGui::OpenPopup("ComponentSettings");
        }
        ImGui::PopStyleVar();

        bool remove_component = false;
        if (ImGui::BeginPopup("ComponentSettings")) {
            if (ImGui::MenuItem("删除组件")) {
                remove_component = true;
            }
            ImGui::EndPopup();
        }

        if (is_open) {
            auto& color = entity.getComponent<SpriteRendererComponent>().Color;
            ImGui::ColorEdit4("颜色", glm::value_ptr(color));
            ImGui::TreePop();
            ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y * 2.0f));  // 空两倍默认间距
        }

        if (remove_component) {
            entity.removeComponent<SpriteRendererComponent>();
        }
    }
}

}  // namespace Hazel