#pragma once

#include <vector>
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Delegate.hpp"

struct Particle2D;
class GPUMesh;
class ShaderState;
class SpriteSheet;
class Clock;
class Texture;
class ParticleSystem2D;
class RenderContext;
class RandomNumberGenerator;

class Emitter2D
{
    friend class ParticleSystem2D;

public:
    Delegate<float, float, Vec2&, Rgba8&> onParticleGrowOlder;

public:
    Emitter2D(unsigned int particleNum,Vec2 const& position, Vec2 const& velocity, float orientationDegrees,
        Vec2 const& initScale, float maxAge, Vec2 const& acceleration, Rgba8 const& tint,
        ShaderState const* shaderState, Texture const* diffuseTex, 
        FloatRange deltaStartOrientDegrees, FloatRange deltaStartSpeed,
        FloatRange deltaStartScaleFraction, FloatRange deltaMaxAgeFraction);
    ~Emitter2D();

    void Update();
    void Render();

    void StopAndClear();

private:
    void UpdateParticles();
    void UpdateForMesh();

private:
    std::vector<Particle2D> m_particles;

    bool m_enabled = true;
    bool m_isGarbage = false;

    //TODO burst based
    //int m_particleNumPerBurst = 0;
    //int m_burstPerSecond = 0;

    Vec2  m_startPos;
    Vec2  m_velocity;
    Vec2  m_acceleration;
    Vec2  m_initScale;
    float m_orientDegrees = 0.f;
    float m_maxAge = 1.f;
    Rgba8 m_tint;
    FloatRange m_deltaStartOrientDegrees;
    FloatRange m_deltaStartSpeed;
    FloatRange m_deltaStartScaleFraction;
    FloatRange m_deltaMaxAgeFraction;

    RandomNumberGenerator* m_rng = nullptr;
    RenderContext* m_ctx = nullptr;
    Clock* m_clock = nullptr;
    GPUMesh* m_mesh = nullptr;
    ShaderState const* m_shaderState = nullptr;
    Texture const* m_texture = nullptr;
};