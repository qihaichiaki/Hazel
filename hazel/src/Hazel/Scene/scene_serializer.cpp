#include "hzpch.h"
#include "scene_serializer.h"
#include <yaml-cpp/yaml.h>

#include "Hazel/Scene/entity.h"
#include "Hazel/Scene/components.h"

// YAML 反序列化特化
namespace YAML
{
template <>
struct convert<glm::vec2>
{
    static Node encode(const glm::vec2& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        return node;
    }

    static bool decode(const Node& node, glm::vec2& rhs)
    {
        if (!node.IsSequence() || node.size() != 2) {
            return false;
        }

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();

        return true;
    }
};

template <>
struct convert<glm::vec3>
{
    static Node encode(const glm::vec3& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        return node;
    }

    static bool decode(const Node& node, glm::vec3& rhs)
    {
        if (!node.IsSequence() || node.size() != 3) {
            return false;
        }

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();

        return true;
    }
};

template <>
struct convert<glm::vec4>
{
    static Node encode(const glm::vec4& rhs)
    {
        Node node;
        node.push_back(rhs.r);
        node.push_back(rhs.g);
        node.push_back(rhs.b);
        node.push_back(rhs.a);
        return node;
    }

    static bool decode(const Node& node, glm::vec4& rhs)
    {
        if (!node.IsSequence() || node.size() != 4) {
            return false;
        }

        rhs.r = node[0].as<float>();
        rhs.g = node[1].as<float>();
        rhs.b = node[2].as<float>();
        rhs.a = node[3].as<float>();

        return true;
    }
};
}  // namespace YAML

namespace Hazel
{

static inline std::string bodyTypeToString(Rigidbody2DComponent::BodyType body_type)
{
    switch (body_type) {
        case Rigidbody2DComponent::BodyType::Static:
            return "Static";
        case Rigidbody2DComponent::BodyType::Dynamic:
            return "Dynamic";
        case Rigidbody2DComponent::BodyType::Kinematic:
            return "Kinematic";
    }

    HZ_CORE_ASSERT(false, "不存在这样的BodyType类型");
    return "Static";
}

static inline Rigidbody2DComponent::BodyType stringToBodyType(const std::string& body_tyoe_str)
{
    if (body_tyoe_str == "Static") {
        return Rigidbody2DComponent::BodyType::Static;
    } else if (body_tyoe_str == "Dynamic") {
        return Rigidbody2DComponent::BodyType::Dynamic;
    } else if (body_tyoe_str == "Kinematic") {
        return Rigidbody2DComponent::BodyType::Kinematic;
    }

    HZ_CORE_ASSERT(false, "不存在这样的BodyType类型");
    return Rigidbody2DComponent::BodyType::Static;
}

// YAML 序列化特化
YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
{
    out << YAML::Flow;  // 数组变成[], 而不是一行一行
    out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
{
    out << YAML::Flow;  // 数组变成[], 而不是一行一行
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
{
    out << YAML::Flow;  // 数组变成[], 而不是一行一行
    out << YAML::BeginSeq << v.r << v.g << v.b << v.a << YAML::EndSeq;
    return out;
}

SceneSerializer::SceneSerializer(const Ref<Scene>& scene) : m_scene{scene} {}

static void serializeEntity(YAML::Emitter& out, Entity entity)
{
    out << YAML::BeginMap;
    out << YAML::Key << "Entity" << YAML::Value << (uint64_t)(entity.getUUID());

    // tag
    if (entity.hasComponent<TagComponent>()) {
        out << YAML::Key << "TagComponent";
        out << YAML::BeginMap;  // TagComponent

        out << YAML::Key << "Tag" << YAML::Value << entity.getComponent<TagComponent>().Tag;
        out << YAML::EndMap;  // TagComponent
    }

    // transform
    if (entity.hasComponent<TransformComponent>()) {
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap;  // TransformComponent

        auto& tc = entity.getComponent<TransformComponent>();
        out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
        out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
        out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

        out << YAML::EndMap;  // TransformComponent
    }

    // CameraComponent
    if (entity.hasComponent<CameraComponent>()) {
        out << YAML::Key << "CameraComponent";
        out << YAML::BeginMap;  // CameraComponent

        auto& camera_component = entity.getComponent<CameraComponent>();
        out << YAML::Key << "Camera";
        out << YAML::BeginMap;  // Camera
        out << YAML::Key << "ProjectionType" << YAML::Value
            << (int)camera_component.Camera.getProjectionType();
        out << YAML::Key << "PerspectiveFOV" << YAML::Value
            << camera_component.Camera.getPerspectiveFOV();
        out << YAML::Key << "PerspectiveNear" << YAML::Value
            << camera_component.Camera.getPerspectiveNearClip();
        out << YAML::Key << "PerspectiveFar" << YAML::Value
            << camera_component.Camera.getPerspectiveFarClip();
        out << YAML::Key << "OrthographicSize" << YAML::Value
            << camera_component.Camera.getOrthographicSize();
        out << YAML::Key << "OrthographicNear" << YAML::Value
            << camera_component.Camera.getOrthographicNearClip();
        out << YAML::Key << "OrthographicFar" << YAML::Value
            << camera_component.Camera.getOrthographicFarClip();
        out << YAML::EndMap;  // Camera

        out << YAML::Key << "Primary" << YAML::Value << camera_component.Primary;
        out << YAML::Key << "FixedAspectRatio" << YAML::Value << camera_component.FixedAspectRatio;

        out << YAML::EndMap;  // CameraComponent
    }

    // SpriteRendererComponent
    if (entity.hasComponent<SpriteRendererComponent>()) {
        out << YAML::Key << "SpriteRendererComponent";
        out << YAML::BeginMap;  // SpriteRendererComponent

        auto& sprite_renderer = entity.getComponent<SpriteRendererComponent>();
        out << YAML::Key << "Color" << YAML::Value << sprite_renderer.Color;
        out << YAML::EndMap;  // SpriteRendererComponent
    }

    // CircleRendererComponent
    if (entity.hasComponent<CircleRendererComponent>()) {
        out << YAML::Key << "CircleRendererComponent";
        out << YAML::BeginMap;  // SpriteRendererComponent

        auto& circle_renderer = entity.getComponent<CircleRendererComponent>();
        out << YAML::Key << "Color" << YAML::Value << circle_renderer.Color;
        out << YAML::Key << "Thickness" << YAML::Value << circle_renderer.Thickness;
        out << YAML::Key << "Fade" << YAML::Value << circle_renderer.Fade;
        out << YAML::EndMap;  // SpriteRendererComponent
    }

    // Rigidbody2DComponent
    if (entity.hasComponent<Rigidbody2DComponent>()) {
        out << YAML::Key << "Rigidbody2DComponent";
        out << YAML::BeginMap;  // Rigidbody2DComponent

        auto& rb = entity.getComponent<Rigidbody2DComponent>();
        out << YAML::Key << "BodyType" << YAML::Value << bodyTypeToString(rb.Type);
        out << YAML::Key << "FixedRotation" << YAML::Value << rb.FixedRotation;
        out << YAML::EndMap;  // Rigidbody2DComponent
    }

    // BoxCollider2DComponent
    if (entity.hasComponent<BoxCollider2DComponent>()) {
        out << YAML::Key << "BoxCollider2DComponent";
        out << YAML::BeginMap;  // BoxCollider2DComponent

        auto& bc = entity.getComponent<BoxCollider2DComponent>();
        out << YAML::Key << "Offset" << YAML::Value << bc.Offset;
        out << YAML::Key << "Size" << YAML::Value << bc.Size;
        out << YAML::Key << "Density" << YAML::Value << bc.Density;
        out << YAML::Key << "Friction" << YAML::Value << bc.Friction;
        out << YAML::Key << "Restitution" << YAML::Value << bc.Restitution;
        out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc.RestitutionThreshold;
        out << YAML::EndMap;  // BoxCollider2DComponent
    }

    out << YAML::EndMap;
}

void SceneSerializer::serialize(const std::string& filepath)
{
    HZ_CORE_ASSERT(m_scene.get(), "Scene的生命周期已经结束");

    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "Scene name";
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
    // 遍历当前场景所有的entity
    auto view = m_scene->m_registry.view<entt::entity>();

    for (auto it = view.rbegin(); it != view.rend(); ++it) {
        Entity entity{*it, m_scene.get()};
        // TODO: check entity?
        serializeEntity(out, entity);
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream fout{filepath};
    fout << out.c_str();
}

bool SceneSerializer::deserialize(const std::string& filepath)
{
    HZ_CORE_ASSERT(m_scene.get(), "Scene的生命周期已经结束");

    std::ifstream fin{filepath};
    if (!fin.is_open()) {
        HZ_CORE_WARN("SceneSerializer::deserialize {}路径打开文件失败!", filepath.c_str());
        return false;
    }
    std::stringstream ss;
    ss << fin.rdbuf();

    YAML::Node root = YAML::Load(ss.str());
    auto scene_name = root["Scene"];
    if (!scene_name) {
        HZ_CORE_WARN("SceneSerializer::deserialize 该{}文件似乎损坏, 未能找到Scene标识",
                     filepath.c_str());
        return false;
    } else {
        HZ_CORE_TRACE("读取场景{}文件成功", scene_name.as<std::string>().c_str());
    }

    auto entities_node = root["Entities"];
    if (entities_node) {
        for (auto entity_node : entities_node) {
            uint64_t uuid = entity_node["Entity"].as<uint64_t>();

            std::string tag_name;
            auto tag_component_node = entity_node["TagComponent"];
            if (tag_component_node) {
                tag_name = tag_component_node["Tag"].as<std::string>();
            }

            HZ_CORE_TRACE("读取实体[id: {}, tag: {}]成功", uuid, tag_name);
            // create entity
            Entity deserialize_entity = m_scene->createEntityWithUUID(uuid, tag_name);

            // TransformComponent
            auto transform_component_node = entity_node["TransformComponent"];
            if (transform_component_node) {
                auto& transform_component = deserialize_entity.getComponent<TransformComponent>();
                transform_component.Translation =
                    transform_component_node["Translation"].as<glm::vec3>();
                transform_component.Rotation = transform_component_node["Rotation"].as<glm::vec3>();
                transform_component.Scale = transform_component_node["Scale"].as<glm::vec3>();
            }

            // CameraComponent
            auto camera_component_node = entity_node["CameraComponent"];
            if (camera_component_node) {
                auto& camera_component = deserialize_entity.addComponent<CameraComponent>();

                // Camera
                auto camera_node = camera_component_node["Camera"];
                if (camera_node) {
                    camera_component.Camera.setProjectionType(
                        (SceneCamera::ProjectionType)(camera_node["ProjectionType"].as<int>()));
                    camera_component.Camera.setPerspectiveFOV(
                        camera_node["PerspectiveFOV"].as<float>());
                    camera_component.Camera.setPerspectiveNearClip(
                        camera_node["PerspectiveNear"].as<float>());
                    camera_component.Camera.setPerspectiveFarClip(
                        camera_node["PerspectiveFar"].as<float>());
                    camera_component.Camera.setOrthographicSize(
                        camera_node["OrthographicSize"].as<float>());
                    camera_component.Camera.setOrthographicNearClip(
                        camera_node["OrthographicNear"].as<float>());
                    camera_component.Camera.setOrthographicFarClip(
                        camera_node["OrthographicFar"].as<float>());
                }

                camera_component.Primary = camera_component_node["Primary"].as<bool>();
                camera_component.FixedAspectRatio =
                    camera_component_node["FixedAspectRatio"].as<bool>();
            }

            // SpriteRendererComponent
            auto sprite_renderer_component_node = entity_node["SpriteRendererComponent"];
            if (sprite_renderer_component_node) {
                auto& sprite_renderer_component =
                    deserialize_entity.addComponent<SpriteRendererComponent>();

                sprite_renderer_component.Color =
                    sprite_renderer_component_node["Color"].as<glm::vec4>();
            }

            // CircleRendererComponent
            auto circle_renderer_component_node = entity_node["CircleRendererComponent"];
            if (circle_renderer_component_node) {
                auto& circle_renderer_component =
                    deserialize_entity.addComponent<CircleRendererComponent>();

                circle_renderer_component.Color =
                    circle_renderer_component_node["Color"].as<glm::vec4>();
                circle_renderer_component.Thickness =
                    circle_renderer_component_node["Thickness"].as<float>();
                circle_renderer_component.Fade = circle_renderer_component_node["Fade"].as<float>();
            }

            // Rigidbody2DComponent
            auto rigidbody_2d_component_node = entity_node["Rigidbody2DComponent"];
            if (rigidbody_2d_component_node) {
                auto& rigidbody_2d_component =
                    deserialize_entity.addComponent<Rigidbody2DComponent>();

                rigidbody_2d_component.Type =
                    stringToBodyType(rigidbody_2d_component_node["BodyType"].as<std::string>());
                rigidbody_2d_component.FixedRotation =
                    rigidbody_2d_component_node["FixedRotation"].as<bool>();
            }

            // BoxCollider2DComponent
            auto box_collider_2d_component_node = entity_node["BoxCollider2DComponent"];
            if (box_collider_2d_component_node) {
                auto& box_collider_2d_component =
                    deserialize_entity.addComponent<BoxCollider2DComponent>();

                box_collider_2d_component.Offset =
                    box_collider_2d_component_node["Offset"].as<glm::vec2>();
                box_collider_2d_component.Size =
                    box_collider_2d_component_node["Size"].as<glm::vec2>();
                box_collider_2d_component.Density =
                    box_collider_2d_component_node["Density"].as<float>();
                box_collider_2d_component.Friction =
                    box_collider_2d_component_node["Friction"].as<float>();
                box_collider_2d_component.Restitution =
                    box_collider_2d_component_node["Restitution"].as<float>();
                box_collider_2d_component.RestitutionThreshold =
                    box_collider_2d_component_node["RestitutionThreshold"].as<float>();
            }
        }
    }

    return true;
}
}  // namespace Hazel