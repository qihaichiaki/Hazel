#pragma once

#include <entt.hpp>

#include <Hazel/Core/timestep.h>
#include <Hazel/Renderer/editor_camera.h>

namespace Hazel
{

// 物理世界的相关句柄
namespace Physic
{
struct B2WorldHandle
{
    uint16_t index1;
    uint16_t generation;
};
}  // namespace Physic

class Entity;
class Scene
{
public:
    HAZEL_API Scene();
    HAZEL_API ~Scene();

    /// @brief 通过当前场景创建一个实体对象
    /// @param name 实体对象名字
    /// @return 实体对象
    /// @note 会默认添加transform组件和tag组件
    HAZEL_API Entity createEntity(const std::string& name = std::string{});

    /// @brief 从当前场景删除实体对象
    /// @param entity 实体对象
    HAZEL_API void destoryEntity(Entity entity);

    /// @brief 场景编辑时更新
    /// @param ts 帧间隔
    /// @warning 临时区分场景运行时和编辑器时的接口
    HAZEL_API void onUpdateEditor(Timestep ts, const EditorCamera& editor_camera);

    /// @brief 开始场景运行时
    HAZEL_API void onStartRuntime();

    /// @brief 场景运行时更新
    /// @param ts 帧间隔
    /// @warning 临时区分场景运行时和编辑器时的接口
    HAZEL_API void onUpdateRuntime(Timestep ts);

    /// @brief 结束场景运行时
    HAZEL_API void onStopRuntime();

    /// @brief 更新场景的视口宽度和高度
    HAZEL_API void onViewportResize(uint32_t width, uint32_t height);

    /// @brief 按照视图获取第一个要渲染的相机对象
    /// @return 相机对象
    /// @warning 后续可能会废弃掉, 当前只是支持获取运行时存在渲染的相机对象
    HAZEL_API Entity getPrimaryCameraEntity() const;

private:
    void onCameraComponentAdded(entt::registry& registry, entt::entity entity);

private:
    entt::registry m_registry;             // 注册管理器, 组件和实体的容器
    Physic::B2WorldHandle m_physic_world;  // 物理世界

    uint32_t m_viewport_width;
    uint32_t m_viewport_height;

    friend class Entity;
    friend class SceneSerializer;
    friend class SceneHierarchyPanel;  // 编辑器场景层次面板, todo remove
};
}  // namespace Hazel