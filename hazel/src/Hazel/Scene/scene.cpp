#include "hzpch.h"
#include "scene.h"
#include "entity.h"
#include "components.h"
#include "Hazel/Scene/scriptable_entity.h"
#include <Hazel/Renderer/renderer_2d.h>
#include <box2d/box2d.h>

namespace Hazel
{

namespace Physic
{

static inline b2BodyType bodyTypeTob2BodyType(Rigidbody2DComponent::BodyType body_type)
{
    switch (body_type) {
        case Rigidbody2DComponent::BodyType::Static:
            return b2BodyType::b2_staticBody;
        case Rigidbody2DComponent::BodyType::Dynamic:
            return b2BodyType::b2_dynamicBody;
        case Rigidbody2DComponent::BodyType::Kinematic:
            return b2BodyType::b2_kinematicBody;
    }
    HZ_CORE_ASSERT(false, "未知Rigidbody2DComponent::BodyType类型");
    return b2BodyType::b2_staticBody;
}

}  // namespace Physic

// test
void Scene::onCameraComponentAdded(entt::registry& registry, entt::entity entity)
{
    auto& camera_component = registry.get<CameraComponent>(entity);
    camera_component.Camera.setViewportSize(m_viewport_width, m_viewport_height);
}

Scene::Scene()
{
    // 注册添加组件回调
    // 组件添加时, entity需要同步的场景信息
    m_registry.on_construct<CameraComponent>().connect<&Scene::onCameraComponentAdded>(this);
}
Scene::~Scene() {}

Entity Scene::createEntity(const std::string& name)
{
    Entity entity{m_registry.create(), this};

    // 自动添加id, transform组件和tag组件
    entity.addComponent<IDComponent>();
    entity.addComponent<TransformComponent>();
    auto& tag = entity.addComponent<TagComponent>();
    tag = name.empty() ? "新实体" : name;
    return entity;
}

Entity Scene::createEntityWithUUID(const UUID& uuid, const std::string& name)
{
    Entity entity{m_registry.create(), this};

    // 自动添加id, transform组件和tag组件
    entity.addComponent<IDComponent>(uuid);
    entity.addComponent<TransformComponent>();
    auto& tag = entity.addComponent<TagComponent>();
    tag = name.empty() ? "新实体" : name;
    return entity;
}

void Scene::destoryEntity(Entity entity)
{
    m_registry.destroy(entity);
}

void Scene::onUpdateEditor(Timestep, const EditorCamera& editor_camera)
{
    Renderer2D::beginScene(editor_camera);

    {
        // group 持久化,初始化稍慢，但是后续访问快->内存连续
        auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            auto [transform, sprite_renderer] =
                group.get<TransformComponent, SpriteRendererComponent>(entity);
            // TODO: 是否需要优化, 防止每次都计算transform
            if (sprite_renderer.Texture) {
                Renderer2D::drawQuad(transform.getTransform(), sprite_renderer.Texture,
                                     sprite_renderer.Color, sprite_renderer.TilingFactor,
                                     (int)entity);
            } else {
                Renderer2D::drawQuad(transform.getTransform(), sprite_renderer.Color, (int)entity);
            }
        }
    }

    {
        m_registry.view<TransformComponent, CircleRendererComponent>().each(
            [](auto enid, TransformComponent& transform, CircleRendererComponent& circle_renderer) {
                Renderer2D::drawCircle(transform.getTransform(), circle_renderer.Color,
                                       circle_renderer.Thickness, circle_renderer.Fade, (int)enid);
            });
    }

    // TODO: 渲染物理碰撞盒子
    // {
    //     m_registry.view<TransformComponent, BoxCollider2DComponent>().each(
    //         [](auto enid, TransformComponent& transform, BoxCollider2DComponent& box_collider2d)
    //         {
    //             TransformComponent box_collider_2d_transform{transform};
    //             box_collider_2d_transform.Translation +=
    //                 glm::vec3{box_collider2d.Offset.x, box_collider2d.Offset.y, 0.0f};
    //             box_collider_2d_transform.Scale *=
    //                 glm::vec3{box_collider2d.Size.x, box_collider2d.Size.y, 1.0f};
    //             Renderer2D::drawRect(box_collider_2d_transform.getTransform(),
    //                                  {1.0f, 1.0f, 1.0f, 1.f}, (int)enid);
    //         });

    //     m_registry.view<TransformComponent, CircleCollider2DComponent>().each(
    //         [](auto enid, TransformComponent& transform,
    //            CircleCollider2DComponent& circle_collider2d) {
    //             TransformComponent circle_collider_2d_transform{transform};
    //             circle_collider_2d_transform.Translation +=
    //                 glm::vec3{circle_collider2d.Offset.x, circle_collider2d.Offset.y, 0.0f};
    //             circle_collider_2d_transform.Scale = {circle_collider2d.Radius * 2.0f,
    //                                                   circle_collider2d.Radius * 2.0f, 1.0f};
    //             Renderer2D::drawCircle(circle_collider_2d_transform.getTransform(),
    //                                    {1.0f, 1.0f, 1.0f, 1.f}, 0.025f, 0.005f, (int)enid);
    //         });
    // }

    Renderer2D::endScene();
}

void Scene::onStartRuntime()
{
    // 创建一个全新的物理世界
    b2WorldDef worldDef = b2DefaultWorldDef();
    // worldDef.restitutionThreshold = 0.5f;
    b2WorldId physic_world = b2CreateWorld(&worldDef);

    // 检查当前场景内所有存在2d刚体组件的entity, 去物理世界创建对应的实体
    auto view = m_registry.view<Rigidbody2DComponent>();
    for (auto enid : view) {
        Entity entity{enid, this};

        auto& rb = entity.getComponent<Rigidbody2DComponent>();
        auto& transform = entity.getComponent<TransformComponent>();

        // 创建box2d中的物理实体
        b2BodyDef body_def = b2DefaultBodyDef();
        body_def.type = Physic::bodyTypeTob2BodyType(rb.Type);
        body_def.motionLocks.angularZ = rb.FixedRotation;  // 锁定z轴上的旋转
        body_def.position = {transform.Translation.x, transform.Translation.y};
        body_def.rotation = b2MakeRot(transform.Rotation.z);  // 设置旋转弧度
        b2BodyId b2body = b2CreateBody(physic_world, &body_def);
        rb.B2BodyId = {b2body.index1, b2body.world0, b2body.generation};

        // 如果包含box碰撞体组件
        if (entity.hasComponent<BoxCollider2DComponent>()) {
            auto& bc = entity.getComponent<BoxCollider2DComponent>();

            // 创建多边形形状;
            b2Polygon box =
                b2MakeOffsetBox(transform.Scale.x * bc.Size.x, transform.Scale.y * bc.Size.y,
                                {bc.Offset.x, bc.Offset.y}, b2MakeRot(0));
            b2ShapeDef shape_def = b2DefaultShapeDef();
            shape_def.density = bc.Density;                   // 密度
            shape_def.material.friction = bc.Friction;        // 摩擦力
            shape_def.material.restitution = bc.Restitution;  // 反弹系数
            // TODO: float RestitutionThreshold{0.5f};  // 反弹恢复阈值
            b2CreatePolygonShape(b2body, &shape_def, &box);
        }

        if (entity.hasComponent<CircleCollider2DComponent>()) {
            auto& cc = entity.getComponent<CircleCollider2DComponent>();

            b2Circle circle{{cc.Offset.x, cc.Offset.y}, cc.Radius};
            b2ShapeDef shape_def = b2DefaultShapeDef();
            shape_def.density = cc.Density;                   // 密度
            shape_def.material.friction = cc.Friction;        // 摩擦力
            shape_def.material.restitution = cc.Restitution;  // 反弹系数
            b2CreateCircleShape(b2body, &shape_def, &circle);
        }
    }

    m_physic_world = {physic_world.index1, physic_world.generation};
}

void Scene::onStopRuntime()
{
    b2DestroyWorld({m_physic_world.index1, m_physic_world.generation});
}

void Scene::onUpdateRuntime(Timestep ts)
{
    // update
    // 执行游戏脚本更新
    m_registry.view<NativeScriptComponent>().each([=](auto entity, NativeScriptComponent& nsc) {
        if (!nsc.Instance) {
            // TODO: 后续会转移到场景play相关函数中去
            if (nsc.InstantiateScript) {
                nsc.Instance = nsc.InstantiateScript();
                nsc.Instance->m_entity = {entity, this};
                nsc.Instance->onCreate();
            }
        }
        nsc.Instance->onUpdate(ts);
    });

    // 物理更新
    b2WorldId physic_world{m_physic_world.index1, m_physic_world.generation};
    b2World_Step(physic_world, ts, 4);
    m_registry.view<Rigidbody2DComponent>().each([this](auto enid, Rigidbody2DComponent& rb) {
        Entity entity{enid, this};

        b2BodyId b2body{rb.B2BodyId.index1, rb.B2BodyId.world0, rb.B2BodyId.generation};
        auto pos = b2Body_GetPosition(b2body);
        auto rot = b2Body_GetRotation(b2body);
        auto angle = atan2(rot.s, rot.c);

        auto& transform = entity.getComponent<TransformComponent>();
        transform.Translation = {pos.x, pos.y, transform.Translation.z};
        transform.Rotation.z = angle;
    });

    // renderer
    // 检查所有的相机组件, 提取主相机和其transform, 方便开启场景渲染
    // view 轻量级查询, 不会改变内存布局
    const Camera* main_camera = nullptr;
    glm::mat4 camera_transform;
    auto camera_views = m_registry.view<CameraComponent>();
    for (auto entity : camera_views) {
        const auto& camera = camera_views.get<CameraComponent>(entity);
        const auto& transform = m_registry.get<TransformComponent>(entity);
        if (camera.Primary) {
            main_camera = &camera.Camera;
            camera_transform = transform.getTransform();
            break;
        }
    }

    // 存在主相机, 则开始2d渲染
    if (main_camera != nullptr) {
        Renderer2D::beginScene(*main_camera, camera_transform);

        // renderer quad
        auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            auto [transform, sprite_renderer] =
                group.get<TransformComponent, SpriteRendererComponent>(entity);
            // TODO: 是否需要优化, 防止每次都计算transform
            if (sprite_renderer.Texture) {
                Renderer2D::drawQuad(transform.getTransform(), sprite_renderer.Texture,
                                     sprite_renderer.Color, sprite_renderer.TilingFactor);
            } else {
                Renderer2D::drawQuad(transform.getTransform(), sprite_renderer.Color, (int)entity);
                // Renderer2D::drawRect(transform.getTransform(), sprite_renderer.Color,
                // (int)entity);
            }
        }

        // renderer circle
        m_registry.view<TransformComponent, CircleRendererComponent>().each(
            [](auto enid, TransformComponent& transform, CircleRendererComponent& circle_renderer) {
                Renderer2D::drawCircle(transform.getTransform(), circle_renderer.Color,
                                       circle_renderer.Thickness, circle_renderer.Fade, (int)enid);
            });

        Renderer2D::endScene();
    }
}

void Scene::onViewportResize(uint32_t width, uint32_t height)
{
    m_viewport_width = width;
    m_viewport_height = height;

    // 找到当前场景所有可以设置纵横比的相机组件, 向其设置宽和高
    auto cameras = m_registry.view<CameraComponent>();
    for (auto entity : cameras) {
        auto& camera = cameras.get<CameraComponent>(entity);
        if (!camera.FixedAspectRatio) {
            camera.Camera.setViewportSize(m_viewport_width, m_viewport_height);
        }
    }
}

Entity Scene::getPrimaryCameraEntity() const
{
    auto view = m_registry.view<CameraComponent>();
    Entity camera_entity;
    for (auto entity_id : view) {
        auto& camera_component = view.get<CameraComponent>(entity_id);
        if (camera_component.Primary) {
            camera_entity = Entity{entity_id, const_cast<Scene*>(this)};
            break;
        }
    }
    return camera_entity;
}

template <typename Component>
static void copyComponent(entt::registry& dst_registry,
                          entt::registry& src_registry,
                          const std::unordered_map<UUID, entt::entity>& entt_map)
{
    auto view = src_registry.view<Component>();
    for (auto enid : view) {
        UUID id = src_registry.get<IDComponent>(enid).ID;
        HZ_CORE_ASSERT(entt_map.find(id) != entt_map.end(), "未知的UUID对象");

        entt::entity new_enid = entt_map.at(id);
        dst_registry.emplace_or_replace<Component>(
            new_enid, view.get<Component>(enid));  // 调用各个组件的拷贝构造函数
    }
}

template <typename Component>
static void copyComponent(Entity dst, Entity src)
{
    if (src.hasComponent<Component>()) {
        dst.addOrReplaceComponent<Component>(src.getComponent<Component>());
    }
}

Ref<Scene> Scene::copy(const Ref<Scene>& other)
{
    // copy 场景
    Ref<Scene> new_scene = createRef<Scene>();
    new_scene->m_viewport_width = other->m_viewport_width;
    new_scene->m_viewport_height = other->m_viewport_height;

    // 遍历所有id组件, 创建对应的实体对象, 并且记录uuid和newScene中entt的映射关系
    std::unordered_map<UUID, entt::entity> entt_map;
    auto other_entities_view = other->m_registry.view<IDComponent>();
    for (auto enid_it = other_entities_view.rbegin(); enid_it != other_entities_view.rend();
         ++enid_it) {
        auto& id_component = other->m_registry.get<IDComponent>(*enid_it);
        auto& tag_component = other->m_registry.get<TagComponent>(*enid_it);

        auto new_entity = new_scene->createEntityWithUUID(id_component.ID, tag_component.Tag);
        entt_map.emplace(id_component.ID, (entt::entity)new_entity);
    }

    // 复制组件
    copyComponent<TransformComponent>(new_scene->m_registry, other->m_registry, entt_map);
    copyComponent<SpriteRendererComponent>(new_scene->m_registry, other->m_registry, entt_map);
    copyComponent<CircleRendererComponent>(new_scene->m_registry, other->m_registry, entt_map);
    copyComponent<CameraComponent>(new_scene->m_registry, other->m_registry, entt_map);
    copyComponent<NativeScriptComponent>(new_scene->m_registry, other->m_registry, entt_map);
    copyComponent<Rigidbody2DComponent>(new_scene->m_registry, other->m_registry, entt_map);
    copyComponent<BoxCollider2DComponent>(new_scene->m_registry, other->m_registry, entt_map);
    copyComponent<CircleCollider2DComponent>(new_scene->m_registry, other->m_registry, entt_map);

    return new_scene;
}

Entity Scene::duplicateEntity(Entity entity)
{
    Entity new_entity;
    if (entity) {
        new_entity = createEntity(entity.getName());

        copyComponent<TransformComponent>(new_entity, entity);
        copyComponent<SpriteRendererComponent>(new_entity, entity);
        copyComponent<CircleRendererComponent>(new_entity, entity);
        copyComponent<CameraComponent>(new_entity, entity);
        copyComponent<NativeScriptComponent>(new_entity, entity);
        copyComponent<Rigidbody2DComponent>(new_entity, entity);
        copyComponent<BoxCollider2DComponent>(new_entity, entity);
        copyComponent<CircleCollider2DComponent>(new_entity, entity);
    }
    return new_entity;
}

}  // namespace Hazel