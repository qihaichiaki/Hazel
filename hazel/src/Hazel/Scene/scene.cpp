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

void Scene::onUpdate(Timestep ts)
{
    // TODO: 实例更新entity, 将其渲染提交到renderer2d上
    auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
    for (auto entity : group) {
        const auto& [transform, sprite_renderer] =
            group.get<TransformComponent, SpriteRendererComponent>(entity);
        Renderer2D::drawQuad(transform, sprite_renderer.Color);
    }
}

}  // namespace Hazel