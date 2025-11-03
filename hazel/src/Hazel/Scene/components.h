#pragma once

#include "Hazel/Core/uuid.h"
#include "Hazel/Scene/scene_camera.h"
#include "Hazel/Renderer/texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Hazel
{

struct HAZEL_API IDComponent
{
    UUID ID;

    IDComponent() = default;
    IDComponent(const IDComponent& other) = default;
    IDComponent(const UUID& uuid) : ID{uuid} {}
};

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
    Ref<Texture2D> Texture{nullptr};
    float TilingFactor{1.0f};

    HAZEL_API SpriteRendererComponent() = default;
    HAZEL_API SpriteRendererComponent(const SpriteRendererComponent& other) = default;
};

struct CircleRendererComponent
{
    glm::vec4 Color{1.0f};
    float Thickness{1.0f};  // 0 ~ 1的厚度, 1表示实心圆
    float Fade{0.005f};     // 模糊效果

    CircleRendererComponent() = default;
    CircleRendererComponent(const CircleRendererComponent& other) = default;
};

struct CameraComponent
{
    SceneCamera Camera;
    bool Primary = true;            // 是否时主相机
    bool FixedAspectRatio = false;  // 是否固定纵横比

    HAZEL_API CameraComponent() = default;
    HAZEL_API CameraComponent(const CameraComponent& other) = default;
};

// 前置声明, 需要使用的地方包含ScriptableEntity定义的地方
class ScriptableEntity;
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

// 物理组件
// 2d刚体组件
struct Rigidbody2DComponent
{
    enum class HAZEL_API BodyType {
        Static,    // 完全静止
        Dynamic,   // 运动的
        Kinematic  // 不受力的作用
    };

    BodyType Type{BodyType::Static};
    bool FixedRotation{false};

    struct B2BodyHandle
    {
        int32_t index1;
        uint16_t world0;
        uint16_t generation;
    };
    B2BodyHandle B2BodyId;

    HAZEL_API Rigidbody2DComponent() = default;
    HAZEL_API Rigidbody2DComponent(const Rigidbody2DComponent& other) = default;
};

// 2d方形碰撞箱
struct BoxCollider2DComponent
{
    glm::vec2 Offset{0.0f, 0.0f};
    glm::vec2 Size{0.5f, 0.5f};
    float Density{1.0f};               // 密度
    float Friction{0.5f};              // 摩擦力
    float Restitution{0.0f};           // 反弹系数
    float RestitutionThreshold{0.5f};  // 反弹恢复阈值

    HAZEL_API BoxCollider2DComponent() = default;
    HAZEL_API BoxCollider2DComponent(const BoxCollider2DComponent& other) = default;
};

}  // namespace Hazel