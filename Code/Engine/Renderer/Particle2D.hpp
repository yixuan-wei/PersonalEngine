#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"

struct Particle2D
{
public:
    Particle2D(Vec2 const& pos, float orient, Vec2 const& vel, Vec2 const& initScale,float maxLife, Rgba8 const& tint);

    bool IsGarbage() const;

public:
    Vec2    position; 
    Vec2    scale;
    Vec2    velocity;
    float   rotateDegrees;    
    float   age = 0.f;
    float   maxAge = 1.f;
    Rgba8   color;
};