#include "Engine/Renderer/ParticleSystem2D.hpp"
#include "Engine/Renderer/Emitter2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"

ParticleSystem2D* ParticleSystem2D::gParticleSystem2D = nullptr;

//////////////////////////////////////////////////////////////////////////
ParticleSystem2D::ParticleSystem2D(RenderContext* renderContext)
    : m_ctx(renderContext)
{
    if (gParticleSystem2D != nullptr) {
        ERROR_AND_DIE("Multiple particle system initialized");
    }

    gParticleSystem2D = this;
}

//////////////////////////////////////////////////////////////////////////
ParticleSystem2D::~ParticleSystem2D()
{
    for (Emitter2D* e : m_emitters) {
        if (e != nullptr) {
            delete e;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
Emitter2D* ParticleSystem2D::StartEmitter(unsigned int particleNum,Vec2 const& position, 
    Vec2 const& velocity, float orientationDegrees, Vec2 const& initScale, 
    float maxAge /*= 1.f*/, Vec2 const& acceleration /*= Vec2::ZERO*/, 
    Rgba8 const& tint /*= Rgba8::WHITE*/, ShaderState const* shaderState /*= nullptr*/, 
    Texture const* diffuseTex /*= nullptr*/, 
    FloatRange deltaStartOrientDegrees /*= FloatRange(0.f,180.f)*/,
    FloatRange deltaStartSpeed/*=FloatRange(.1f, 1.f)*/, 
    FloatRange deltaStartScaleFraction/*=FloatRange(1.f)*/, 
    FloatRange deltaMaxAgeFraction/*=FloatRange(1.f) */)
{
    Emitter2D* newEmitter = new Emitter2D(particleNum,position, velocity, orientationDegrees, initScale, maxAge,
        acceleration, tint, shaderState, diffuseTex, deltaStartOrientDegrees, deltaStartSpeed,
        deltaStartScaleFraction, deltaMaxAgeFraction);
   
   for (size_t i = 0; i < m_emitters.size(); i++) {
        if (m_emitters[i] == nullptr) {
            m_emitters[i] = newEmitter;
            return newEmitter;
        }
    }
    m_emitters.push_back(newEmitter);
    return newEmitter;
}

//////////////////////////////////////////////////////////////////////////
void ParticleSystem2D::Update()
{
    for (size_t i = 0; i < m_emitters.size(); i++) {
        Emitter2D* e = m_emitters[i];
        if (e == nullptr) {
            continue;
        }

        if (e->m_isGarbage) {
            delete e;
            m_emitters[i] = nullptr;
        }
        else {
            e->Update();
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void ParticleSystem2D::Render()
{
    for (size_t i = 0; i < m_emitters.size(); i++) {
        Emitter2D* e = m_emitters[i];
        if (e != nullptr) {
            e->Render();
        }
    }
}

