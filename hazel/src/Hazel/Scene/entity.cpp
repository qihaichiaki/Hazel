#include "hzpch.h"
#include "entity.h"

namespace Hazel
{
Entity::Entity() = default;
Entity::Entity(entt::entity entity_handle, Scene* scene)
    : m_entity_handle{entity_handle}, m_scene{scene}
{
}
Entity::Entity(const Entity& other) = default;
Entity::~Entity() = default;

}  // namespace Hazel