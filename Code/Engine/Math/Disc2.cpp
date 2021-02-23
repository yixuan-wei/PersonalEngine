#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/MathUtils.hpp"

//////////////////////////////////////////////////////////////////////////
Disc2::Disc2(Vec2 const& c, float r)
    :center(c)
    ,radius(r)
{
}

//////////////////////////////////////////////////////////////////////////
bool Disc2::IsPointInside(const Vec2& point) const
{
    float distSquared = GetDistanceSquared2D(point, center);
    if (distSquared < radius * radius) {
        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Disc2::GetNearestPoint(const Vec2& point) const
{
    Vec2 toPoint = (point - center).GetClamped(radius);
    return center + toPoint;
}

