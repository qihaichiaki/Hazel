#pragma once

/**
 * 简易粒子系统, 使用hazel进行渲染
 */

#include <glm/glm.hpp>
#include <Hazel/Core/timestep.h>
#include <Hazel/Renderer/ortho_graphic_camera.h>

// 发射/创建粒子结构体
struct ParticleProps
{
    glm::vec2 Position;

    glm::vec2 Velocity;
    glm::vec2 VelocityVariation;

    glm::vec4 ColorBegin;
    glm::vec4 ColorEnd;

    float SizeBegin;
    float SizeVariation;
    float SizeEnd;

    float LifeTime;
};

class ParticleSystem
{
public:
    ParticleSystem(uint32_t particle_index_max = 100000);
    ~ParticleSystem();

    void onUpdate(Hazel::Timestep ts);
    void onRender(const Hazel::OrthoGraphicCamera& camera);

    /// @brief 发射粒子
    void emit(const ParticleProps& particle_props);

private:
    // 粒子结构对象
    struct Particle
    {
        glm::vec2 Position;
        float Rotation;
        glm::vec2 Velocity;
        glm::vec4 ColorBegin;
        glm::vec4 ColorEnd;
        float SizeBegin;
        float SizeEnd;
        float LifeTime;
        float LifeRemaing;
        bool Active = false;
    };

    std::vector<Particle> m_particle_pool;
    uint32_t m_particle_index;  // 当前需要激活的粒子对象下标
};