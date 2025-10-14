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
    flags |= ImGuiTreeNodeFlags_SpanAvailWidth;  // 让标签占满一行，用户更容易点击
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
        ImGuiTreeNodeFlags _flags =
            ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
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
    auto& io = ImGui::GetIO();
    // 绘制当前组件使用粗体
    auto bold_font = io.Fonts->Fonts[0];

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
    ImGui::PushFont(bold_font);
    if (ImGui::Button("X", button_size)) {
        values.x = reset_value;
        is_col = true;
    }
    ImGui::PopFont();
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
    ImGui::PushFont(bold_font);
    if (ImGui::Button("Y", button_size)) {
        values.y = reset_value;
        is_col = true;
    }
    ImGui::PopFont();
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
    ImGui::PushFont(bold_font);
    if (ImGui::Button("Z", button_size)) {
        values.z = reset_value;
        is_col = true;
    }
    ImGui::PopFont();
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

template <typename T, typename UIFunction>
static void drawComponent(const std::string& name,
                          Entity entity,
                          UIFunction ui_function,
                          bool is_no_delete = false)
{
    // 默认展开 | 运行项目重叠（目的时在treenode这一行继续添加控件） | 整一行收标签控制 |
    // 整一行被填充背景色(和当前行元素高度自适应) | 添加框的内边距
    const ImGuiTreeNodeFlags tree_node_flags =
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap |
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_FramePadding;

    if (entity.hasComponent<T>()) {
        auto& component = entity.getComponent<T>();
        ImVec2 content_region_available = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
        auto gimgui = GetImGuiContextPtr();
        float line_height = gimgui->FontSize + gimgui->Style.FramePadding.y * 2.0f;  // 计算行高
        ImGui::Separator();
        bool is_open =
            ImGui::TreeNodeEx((void*)typeid(T).hash_code(), tree_node_flags, name.c_str());
        bool is_render_button = false;
        // 判断鼠标是否悬浮在这个 TreeNode 的矩形区域上上
        if (bool hovered =
                ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) {
            // 鼠标悬浮在 TreeNode 上
            is_render_button = true;
        }
        ImGui::PopStyleVar();

        bool remove_component = false;
        if (is_render_button) {
            ImGui::SameLine(content_region_available.x - line_height * 0.5f);
            if (ImGui::Button("•••", ImVec2{line_height, line_height}))  // 扩展按钮
            {
                ImGui::OpenPopup("ComponentSettings");
            }
        }

        if (ImGui::BeginPopup("ComponentSettings")) {
            if (ImGui::MenuItem("删除组件", 0, false, !is_no_delete)) {
                remove_component = true;
            }
            ImGui::EndPopup();
        }

        if (is_open) {
            // 独特的组件渲染信息内容
            ui_function(component);
            ImGui::TreePop();
            ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y * 2.0f));  // 空两倍默认间距
        }

        if (remove_component) {
            entity.removeComponent<T>();
        }
    }
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
    }

    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    if (ImGui::Button("添加组件")) {
        // 添加组件打开对应id的上下文窗口
        ImGui::OpenPopup("AddComponent");
    }

    if (ImGui::BeginPopup("AddComponent")) {
        if (ImGui::MenuItem("Camera", 0, false,
                            !m_selection_context.hasComponent<CameraComponent>())) {
            m_selection_context.addComponent<CameraComponent>();
            // ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("SpriteRenderer", 0, false,
                            !m_selection_context.hasComponent<SpriteRendererComponent>())) {
            m_selection_context.addComponent<SpriteRendererComponent>();
        }
        ImGui::EndPopup();
    }
    ImGui::PopItemWidth();
    ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y * 2.0f));  // 空两倍默认间距

    // transform
    drawComponent<TransformComponent>(
        "Transform", entity,
        [](TransformComponent& transform_component) {
            drawVec3Contol("位置", transform_component.Translation);
            glm::vec3 d_roation = glm::degrees(transform_component.Rotation);
            if (drawVec3Contol("旋转", d_roation)) {
                transform_component.Rotation = glm::radians(d_roation);
            }
            drawVec3Contol("缩放", transform_component.Scale, 1.0f);
        },
        true);

    // Camera
    drawComponent<CameraComponent>("Camera", entity, [](CameraComponent& camera_component) {
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
            float fov_degree = glm::degrees(camera_component.Camera.getPerspectiveFOV());
            if (ImGui::DragFloat("垂直视野", &fov_degree)) {
                camera_component.Camera.setPerspectiveFOV(glm::radians(fov_degree));
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
    });

    // SpriteRenderer
    drawComponent<SpriteRendererComponent>(
        "Sprite", entity, [](SpriteRendererComponent& sprite_renderer) {
            ImGui::ColorEdit4("颜色", glm::value_ptr(sprite_renderer.Color));
        });
}

}  // namespace Hazel