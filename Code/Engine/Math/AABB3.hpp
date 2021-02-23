#pragma once

#include "Engine/Math/Vec3.hpp"

struct AABB3 
{
public:
    Vec3 mins;
    Vec3 maxs;

public:
    AABB3() = default;
    ~AABB3() = default;
    AABB3(AABB3 const& copyFrom);
    explicit AABB3(Vec3 const& inMins, Vec3 const& inMaxs);

    bool  IsPointInside(Vec3 const& point) const;
    Vec3  GetCenter() const;
    Vec3  GetDimensions() const;
    float GetOutterRadius() const;
    void  GetCornerPoints(Vec3* outEightPoints) const;

    void  Translate(const Vec3& translation);
    void  SetCenter(const Vec3& newCenter);

    bool operator==(AABB3 const& compareWith) const;
    void operator=(AABB3 const& assignFrom);
};