#pragma once

/**
 * 原生脚本对象基类
 */

#include "Hazel/Scene/entity.h"
#include "Hazel/Core/timestep.h"

namespace Hazel
{

class ScriptableEntity
{
public:
    HAZEL_API ScriptableEntity() = default;
    HAZEL_API virtual ~ScriptableEntity() = default;

    /// @brief 获取当前附加实体对象的组件
    /// @tparam T 组件类型
    /// @return 组件对象
    template <typename T>
    T& getComponent()
    {
        return m_entity.getComponent<T>();
    }

    HAZEL_API virtual void onCreate() {}
    HAZEL_API virtual void onDestroy() {}
    HAZEL_API virtual void onUpdate(Timestep) {}

private:
    Entity m_entity;
    friend class Scene;
};
}  // namespace Hazel