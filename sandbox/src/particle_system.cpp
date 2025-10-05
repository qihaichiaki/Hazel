#include "particle_system.h"
#include "random.h"
#include <Hazel/Renderer/renderer_2d.h>

#include <glm/ext/scalar_constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL  // 实验模块包含
#include <glm/gtx/compatibility.hpp>

ParticleSystem::ParticleSystem()
{
    m_particle_pool.resize(1000);  // 硬编码1000个粒子
}

ParticleSystem::~ParticleSystem() {}

void ParticleSystem::onUpdate(Hazel::Timestep ts)
{
    for (auto& particle : m_particle_pool) {
        if (!particle.Active) {
            continue;
        }

        if (particle.LifeRemaing <= 0.0f) {
            particle.Active = false;
            continue;
        }

        particle.LifeRemaing -= ts;
        particle.Position += particle.Velocity * (float)ts;
        particle.Rotation += 0.01f * ts;
    }
}

void ParticleSystem::onRender()
{
    for (auto& particle : m_particle_pool) {
        if (!particle.Active) {
            continue;
        }

        // 插值
        float life = particle.LifeRemaing / particle.LifeTime;
        auto color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);
        color.a *= life;
        float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

        // 提交渲染任务
        Hazel::Renderer2D::drawRotatedQuad(particle.Position, {size, size}, particle.Rotation,
                                           color);
    }
}

void ParticleSystem::emit(const ParticleProps& particle_props)
{
    // NODE: 如果一次发射很多粒子, 超过了上限, 那么这里在发射是复用的之前的粒子
    ParticleSystem::Particle& particle = m_particle_pool[m_particle_index];
    particle.Position = particle_props.Position;
    particle.Rotation = Random::getFloat() * 2.0f * glm::pi<float>();  // 0 ~ 2pi
    // Velocity
    particle.Velocity = particle_props.Velocity;
    particle.Velocity.x += particle_props.VelocityVariation.x * (Random::getFloat() - 0.5f);
    particle.Velocity.y += particle_props.VelocityVariation.y * (Random::getFloat() - 0.5f);
    // color
    particle.ColorBegin = particle_props.ColorBegin;
    particle.ColorEnd = particle_props.ColorEnd;
    // size
    particle.SizeBegin =
        particle_props.SizeBegin + particle_props.SizeVariation * (Random::getFloat() - 0.5f);
    particle.SizeEnd = particle_props.SizeEnd;
    // life
    particle.LifeTime = particle_props.LifeTime;
    particle.LifeRemaing = particle_props.LifeTime;
    particle.Active = true;

    m_particle_index =
        m_particle_index == 0 ? (uint32_t)m_particle_pool.size() - 1 : --m_particle_index;
}