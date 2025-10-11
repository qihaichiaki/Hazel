#pragma once

#include <glm/glm.hpp>

#include "Hazel/Scene/scene_camera.h"
#include "Hazel/Scene/scriptable_entity.h"

namespace Hazel
{

struct TagComponent
{
    std::string Tag;

    HAZEL_API TagComponent() = default;
    HAZEL_API TagComponent(const TagComponent& other) = default;
    HAZEL_API TagComponent(const std::string& tag) : Tag{tag} {}

    HAZEL_API operator std::string&()
    {
        return Tag;
    }
    HAZEL_API operator const std::string&() const
    {
        return Tag;
    }
};

struct TransformComponent
{
    glm::mat4 Transform{1.0f};

    HAZEL_API TransformComponent() = default;
    HAZEL_API TransformComponent(const TransformComponent& other) = default;
    HAZEL_API TransformComponent(const glm::mat4& transform) : Transform{transform} {}

    HAZEL_API operator glm::mat4&()
    {
        return Transform;
    }
    HAZEL_API operator const glm::mat4&() const
    {
        return Transform;
    }
};

struct SpriteRendererComponent
{
    glm::vec4 Color{1.0f};

    HAZEL_API SpriteRendererComponent() = default;
    HAZEL_API SpriteRendererComponent(const SpriteRendererComponent& other) = default;
    HAZEL_API SpriteRendererComponent(const glm::vec4& color) : Color{color} {}
};

struct CameraComponent
{
    SceneCamera Camera;
    bool Primary = true;            // 是否时主相机
    bool FixedAspectRatio = false;  // 是否固定纵横比

    HAZEL_API CameraComponent() = default;
    HAZEL_API CameraComponent(const CameraComponent& other) = default;
};

// 原生脚本组件
struct NativeScriptComponent
{
    ScriptableEntity* Instance{nullptr};

    ScriptableEntity* (*InstantiateScript)() = nullptr;       // 延迟创建脚本对象函数
    void (*DestroyScript)(NativeScriptComponent*) = nullptr;  // 销毁脚本对象函数

    /// @brief 绑定实际的脚本对象
    /// @tparam T 派生的脚本对象类型
    template <typename T>
    std::enable_if_t<std::is_base_of_v<ScriptableEntity, T>, void> bind()
    {
        InstantiateScript = []() -> ScriptableEntity* { return new T{}; };
        DestroyScript = [](NativeScriptComponent* nsc) {
            delete nsc->Instance;
            nsc->Instance = nullptr;
        };
    }

    HAZEL_API NativeScriptComponent() = default;
    HAZEL_API NativeScriptComponent(const NativeScriptComponent& other) = default;
};

}  // namespace Hazel