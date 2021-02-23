#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/LineSegment2.hpp"

struct Manifold2
{
    LineSegment2 contact;
    Vec2 normal;
    float penetration = 0.f;

    Vec2 normalImpulse;
    Vec2 bounceImpulse;
    Vec2 tangentImpulse;

    Manifold2() = default;
    //Manifold2(Vec2 normalDir, float penetrateLength);
    Manifold2(Manifold2 const& manifold);
    Manifold2(Vec2 contact, Vec2 normalDir, float penetrateLength);
    Manifold2(Vec2 contact1, Vec2 contact2, Vec2 normalDir, float penetrateLength);

    Vec2 GetContact() const;
};
