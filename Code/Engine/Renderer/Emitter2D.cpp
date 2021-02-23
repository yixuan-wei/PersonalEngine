#include "Engine/Renderer/Emitter2D.hpp"
#include "Engine/Renderer/ParticleSystem2D.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Particle2D.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/MeshUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


//////////////////////////////////////////////////////////////////////////
Emitter2D::Emitter2D(unsigned int particleNum, Vec2 const& position, Vec2 const& velocity, float orientationDegrees,
    Vec2 const& initScale, float maxAge, Vec2 const& acceleration, Rgba8 const& tint, 
    ShaderState const* shaderState, Texture const* diffuseTex, 
    FloatRange deltaStartOrientDegrees, FloatRange deltaStartSpeed, 
    FloatRange deltaStartScaleFraction, FloatRange deltaMaxAgeFraction)
    : m_startPos(position)
    , m_velocity(velocity)
    , m_orientDegrees(orientationDegrees)
    , m_initScale(initScale)
    , m_maxAge(maxAge)
    , m_tint(tint)
    , m_shaderState(shaderState)
    , m_texture(diffuseTex)
    , m_acceleration(acceleration)
    , m_deltaMaxAgeFraction(deltaMaxAgeFraction)
    , m_deltaStartOrientDegrees(deltaStartOrientDegrees)
    , m_deltaStartScaleFraction(deltaStartScaleFraction)
    , m_deltaStartSpeed(deltaStartSpeed)
{
    if (ParticleSystem2D::gParticleSystem2D == nullptr) {
        ERROR_AND_DIE("No particle system existed when emitter initializing");
    }

    m_clock = new Clock();
    m_rng = new RandomNumberGenerator();

    m_ctx = ParticleSystem2D::gParticleSystem2D->GetRenderContext();
    m_mesh = new GPUMesh(m_ctx);

    if (maxAge == 0.f) {
        ERROR_RECOVERABLE("Emitter initiated with maxAge 0");
        return;
    }

    size_t totalParticleNum = particleNum > MAX_PARTICLE_PER_EMITTER ? MAX_PARTICLE_PER_EMITTER : particleNum;
    m_particles.reserve(totalParticleNum);
    for (size_t i = 0; i < totalParticleNum; i++) {
        float orientDir = m_rng->RollRandomFloatZeroToAlmostOne()>=.5f ? 1.f:-1.f;
        float orientDegrees = m_orientDegrees + orientDir * m_deltaStartOrientDegrees.GetRandomInRange(*m_rng);
        Vec2 startDir = Vec2::MakeFromPolarDegrees(orientDegrees);
        m_particles.emplace_back(m_startPos, 
            orientDegrees, 
            m_velocity + startDir*m_deltaStartSpeed.GetRandomInRange(*m_rng), 
            m_initScale*m_deltaStartScaleFraction.GetRandomInRange(*m_rng), 
            m_maxAge*m_deltaMaxAgeFraction.GetRandomInRange(*m_rng), 
            m_tint);
    }
}

//////////////////////////////////////////////////////////////////////////
Emitter2D::~Emitter2D()
{
    delete m_mesh;
    delete m_clock;
}

//////////////////////////////////////////////////////////////////////////
void Emitter2D::Update()
{
    UpdateParticles();
    UpdateForMesh();
}

//////////////////////////////////////////////////////////////////////////
void Emitter2D::Render()
{
    m_ctx->BindShaderState(m_shaderState);
    m_ctx->BindDiffuseTexture(m_texture);
    m_ctx->DrawMesh(m_mesh);
}

//////////////////////////////////////////////////////////////////////////
void Emitter2D::StopAndClear()
{
    m_isGarbage = true;
}

//////////////////////////////////////////////////////////////////////////
void Emitter2D::UpdateParticles()
{
    float deltaSeconds = (float)m_clock->GetLastDeltaSeconds();
    bool particleExisted = false;
    for (size_t i = 0; i < m_particles.size(); i++) {        
        Particle2D& p = m_particles[i];
        if (p.IsGarbage()) {
            continue;
        }

        particleExisted = true;
        Vec2 deltaVelocity = deltaSeconds * m_acceleration;
        p.velocity += deltaVelocity;
        p.position += p.velocity*deltaSeconds;
        p.age += deltaSeconds;

        onParticleGrowOlder(p.age, p.maxAge, p.scale, p.color);
    }

    if (!particleExisted) {
        m_isGarbage = true;  //TODO auto release
    }
}

//////////////////////////////////////////////////////////////////////////
void Emitter2D::UpdateForMesh()
{
    std::vector<Vertex_PCU> verts;
    std::vector<unsigned int> inds;

    for (size_t i = 0; i < m_particles.size(); i++) {
        Particle2D const& p = m_particles[i];
        if (p.IsGarbage()) {
            continue;
        }

        Vec2 halfScale = p.scale*.5f;
        Vec2 center = p.position;
        TODO("particle orientation");
        /*Vec2 right = Vec2::MakeFromPolarDegrees(p.rotateDegrees);
        Vec2 up = right.GetRotated90Degrees();
        halfScale = halfScale.x*right + halfScale.y*up;*/
        AppendIndexedVertexesForQuaterPolygon2D(verts, inds, center-halfScale, center+Vec2(halfScale.x, -halfScale.y),
            center+halfScale, center+Vec2(-halfScale.x, halfScale.y), p.color, Vec2::ZERO, Vec2::ONE);
    }

    if(verts.size()>0){
        m_mesh->UpdateIndices((unsigned int)inds.size(), &inds[0]);
        m_mesh->UpdateVertices((unsigned int)verts.size(), &verts[0]);
    }
}

