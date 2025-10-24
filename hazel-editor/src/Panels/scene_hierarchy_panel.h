#pragma once

#include <Hazel/Scene/scene.h>
#include <Hazel/Scene/entity.h>

namespace Hazel
{

class SceneHierarchyPanel
{
public:
    SceneHierarchyPanel() = default;

    /// @brief 设置scene对象
    void setContext(const Ref<Scene>& scene)
    {
        m_context = scene;
        m_selection_context = {};
    }

    /// @brief 渲染imgui场景层次
    void onImGuiRenderer();

    /// @brief 返回当前场景面板选择的entity对象
    Entity getSelectedEntity() const
    {
        return m_selection_context;
    }

    void setSelectedEntity(Entity selection_context)
    {
        m_selection_context = selection_context;
    }

private:
    void drawEntityNode(Entity entity);
    void drawComponents(Entity entity);

private:
    Ref<Scene> m_context;
    Entity m_selection_context;
};
}  // namespace Hazel