#pragma once

#include <Hazel/Core/log.h>
#include <Hazel/Scene/scene.h>
#include <Hazel/Scene/components.h>

namespace Hazel
{

class Entity
{
public:
    HAZEL_API Entity();
    HAZEL_API Entity(entt::entity entity_handle, Scene* scene);
    HAZEL_API Entity(const Entity& other);
    HAZEL_API ~Entity();

    /// @brief 查询当前entity是否存在对应的组件
    /// @tparam T 组件类型
    /// @return 是否存在
    template <typename T>
    bool hasComponent()
    {
        return m_scene->m_registry.all_of<T>(m_entity_handle);
    }

    /// @brief 给当前实体对象添加组件对象
    /// @tparam T 组件类型
    /// @tparam ...Arg 组件构造参数类型
    /// @param ...args 组件构造参数值
    /// @return 组件对象
    template <typename T, typename... Arg>
    T& addComponent(Arg&&... args)
    {
        HZ_CORE_ASSERT(!hasComponent<T>(), "当前组件类型已经存在!");
        return m_scene->m_registry.emplace<T>(m_entity_handle, std::forward<Arg>(args)...);
    }

    /// @brief 获取当前实体的组件
    /// @tparam T 组件类型
    /// @return 组件对象
    template <typename T>
    T& getComponent()
    {
        HZ_CORE_ASSERT(hasComponent<T>(), "当前组件类型不存在!");
        return m_scene->m_registry.get<T>(m_entity_handle);
    }

    template <typename T>
    void removeComponent()
    {
        HZ_CORE_ASSERT(hasComponent<T>(), "当前组件类型不存在!");
        m_scene->m_registry.remove<T>(m_entity_handle);
    }

    HAZEL_API operator bool() const
    {
        return m_entity_handle != entt::null && m_scene->m_registry.valid(m_entity_handle);
    }
    HAZEL_API operator uint32_t() const
    {
        return static_cast<uint32_t>(m_entity_handle);
    }
    HAZEL_API operator entt::entity() const
    {
        return m_entity_handle;
    }
    HAZEL_API bool operator==(const Entity& other) const
    {
        return other.m_entity_handle == m_entity_handle && other.m_scene == m_scene;
    }
    HAZEL_API bool operator!=(const Entity& other) const
    {
        return !(other == *this);
    }

    HAZEL_API UUID getUUID()
    {
        return getComponent<IDComponent>().ID;
    }

private:
    entt::entity m_entity_handle{entt::null};
    Scene* m_scene{nullptr};
};

}  // namespace Hazel