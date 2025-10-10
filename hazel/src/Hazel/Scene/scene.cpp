#include "hzpch.h"
#include "scene.h"
#include "entity.h"
#include "components.h"
#include <Hazel/Renderer/renderer_2d.h>

namespace Hazel
{

Scene::Scene() {}
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

void Scene::onUpdate(Timestep)
{
    // 检查所有的相机组件, 提取主相机和其transform, 方便开启场景渲染
    // view 轻量级查询, 不会改变内存布局
    const Camera* main_camera = nullptr;
    const glm::mat4* camera_transform = nullptr;
    auto camera_views = m_registry.view<CameraComponent>();
    for (auto entity : camera_views) {
        const auto& camera = camera_views.get<CameraComponent>(entity);
        const auto& transform = m_registry.get<TransformComponent>(entity);  // TODO: 保留?
        if (camera.Primary) {
            main_camera = &camera.Camera;
            camera_transform = &transform.Transform;
            break;
        }
    }

    // 存在主相机, 则开始2d渲染
    if (main_camera != nullptr) {
        Renderer2D::beginScene(*main_camera, *camera_transform);

        // group 持久化,初始化稍慢，但是后续访问快->内存连续
        auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            const auto& [transform, sprite_renderer] =
                group.get<TransformComponent, SpriteRendererComponent>(entity);
            Renderer2D::drawQuad(transform, sprite_renderer.Color);
        }

        Renderer2D::endScene();
    }
}

void Scene::onViewportResize(uint32_t width, uint32_t height)
{
    // 找到当前场景所有可以设置纵横比的相机组件, 向其设置宽和高
    auto cameras = m_registry.view<CameraComponent>();
    for (auto entity : cameras) {
        auto& camera = cameras.get<CameraComponent>(entity);
        if (!camera.FixedAspectRatio) {
            camera.Camera.setViewportSize(width, height);
        }
    }
}

}  // namespace Hazel