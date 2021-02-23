#include "Engine/Renderer/Particle2D.hpp"

//////////////////////////////////////////////////////////////////////////
Particle2D::Particle2D(Vec2 const& pos, float orient, Vec2 const& vel, Vec2 const& initScale, float maxLife, Rgba8 const& tint)
    : position(pos)
    , rotateDegrees(orient)
    , velocity(vel)
    , scale(initScale)
    , maxAge(maxLife)
    , color(tint)
{

}

//////////////////////////////////////////////////////////////////////////
bool Particle2D::IsGarbage() const
{
    return age>maxAge;
}
