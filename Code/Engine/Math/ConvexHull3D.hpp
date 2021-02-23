#pragma once

#include "Engine/Math/Plane3D.hpp"
#include <vector>

struct Vec3;

class ConvexHull3D
{
public:
    std::vector<Plane3D> m_boundingPlanes;

public:
    bool IsPointInside(Vec3 const& point) const;
};