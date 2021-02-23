#pragma once

#include "Engine/Math/Vec2.hpp"

struct Disc2 
{
public:
    Vec2 center;
    float radius = 0.f;

public:
    Disc2() {}
    ~Disc2() {}
    explicit Disc2(Vec2 const& c, float r);

    //Accessors
    bool IsPointInside(const Vec2& point)const;
    const Vec2 GetNearestPoint(const Vec2& point) const;
};