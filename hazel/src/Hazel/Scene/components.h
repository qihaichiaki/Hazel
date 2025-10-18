#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

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
    // glm::mat4 Transform{1.0f};
    glm::vec3 Translation{0.0f};
    glm::vec3 Rotation{0.0f};
    glm::vec3 Scale{1.0f};

    HAZEL_API TransformComponent() = default;
    HAZEL_API TransformComponent(const TransformComponent& other) = default;

    /// @brief 计算transform矩阵
    glm::mat4 getTransform() const
    {
        // auto rotation = glm::rotate(glm::mat4{1.0f}, Rotation.x, {1.0f, 0.0f, 0.0f}) *
        //                 glm::rotate(glm::mat4{1.0f}, Rotation.y, {0.0f, 1.0f, 0.0f}) *
        //                 glm::rotate(glm::mat4{1.0f}, Rotation.z, {0.0f, 0.0f, 1.0f});
        auto rotation = glm::toMat4(glm::quat(Rotation));  // 使用欧拉角?
        return glm::translate(glm::mat4{1.0f}, Translation) * rotation *
               glm::scale(glm::mat4{1.0f}, Scale);
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