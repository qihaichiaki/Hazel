#pragma once

#include <entt.hpp>

#include <Hazel/Core/timestep.h>

namespace Hazel
{

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

    /// @brief 场景更新
    /// @param ts 帧间隔
    HAZEL_API void onUpdate(Timestep ts);

    /// @brief 更新场景的视口宽度和高度
    HAZEL_API void onViewportResize(uint32_t width, uint32_t height);

private:
    entt::registry m_registry;  // 注册管理器, 组件和实体的容器

    friend class Entity;
};
}  // namespace Hazel