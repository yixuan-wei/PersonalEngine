#pragma once

#include <vector>
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Rgba8.hpp"

class Emitter2D;
class RenderContext;
class ShaderState;
class Texture;

class ParticleSystem2D
{
public:
    static ParticleSystem2D* gParticleSystem2D;

    ParticleSystem2D(RenderContext* renderContext);
    ~ParticleSystem2D();

    Emitter2D* StartEmitter(unsigned int particleNum,Vec2 const& position, Vec2 const& velocity, 
        float orientationDegrees, Vec2 const& initScale, float maxAge = 1.f, 
        Vec2 const& acceleration = Vec2::ZERO, Rgba8 const& tint = Rgba8::WHITE, 
        ShaderState const* shaderState = nullptr, Texture const* diffuseTex = nullptr,
        FloatRange deltaStartOrientDegrees = FloatRange(0.f,90.f),
        FloatRange deltaStartSpeed=FloatRange(.1f, 1.f),
        FloatRange deltaStartScaleFraction=FloatRange(1.f),
        FloatRange deltaMaxAgeFraction=FloatRange(1.f) );

    void Update();
    void Render();

    RenderContext* GetRenderContext() const {return m_ctx;}

private:
    std::vector<Emitter2D*> m_emitters;

    RenderContext* m_ctx = nullptr;

    //TODO share sprite sheet for all particles
    //SpriteSheet* m_spriteSheet = nullptr;
};