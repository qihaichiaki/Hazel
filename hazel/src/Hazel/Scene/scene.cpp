#include "hzpch.h"
#include "scene.h"
#include "entity.h"
#include "components.h"
#include <Hazel/Renderer/renderer_2d.h>

namespace Hazel
{

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

    // 自动添加transform组件和tag组件
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

    // group 持久化,初始化稍慢，但是后续访问快->内存连续
    auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
    for (auto entity : group) {
        auto [transform, sprite_renderer] =
            group.get<TransformComponent, SpriteRendererComponent>(entity);
        // TODO: 是否需要优化, 防止每次都计算transform
        Renderer2D::drawQuad(transform.getTransform(), sprite_renderer.Color, (int)entity);
    }

    Renderer2D::endScene();
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

        // group 持久化,初始化稍慢，但是后续访问快->内存连续
        auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            auto [transform, sprite_renderer] =
                group.get<TransformComponent, SpriteRendererComponent>(entity);
            // TODO: 是否需要优化, 防止每次都计算transform
            Renderer2D::drawQuad(transform.getTransform(), sprite_renderer.Color);
        }

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

}  // namespace Hazel