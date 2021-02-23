#pragma once

#include "Engine/Math/Vec3.hpp"

struct Plane3D
{
public:
    Vec3 normal = Vec3(0.f,0.f,1.f);
    float distanceFromOriginAlongNormal = 0.f;

public:
    Plane3D(Vec3 const& n, Vec3 const& pointOnPlane);

    float GetDistance(Vec3 const& point) const;
    bool IsPointInFront(Vec3 const& point) const;
};