#pragma once

#include "Engine/Math/Plane2D.hpp"
#include <vector>

struct Vec2;

class ConvexHull2D
{
public:
    std::vector<Plane2D> m_boundingPlanes;

public:
    void AddPlane(Vec2 const& normal, float distFromOrigin);

    bool IsPointInside(Vec2 const& point) const;    //include on boundary
    bool HitByRay(Vec2 const& start, Vec2 const& end, Vec2& outHitNormal, Vec2& outHitPoint, float& outLengthFrac) const;
};