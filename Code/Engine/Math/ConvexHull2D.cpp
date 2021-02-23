#include "Engine/Math/ConvexHull2D.hpp"
#include "Engine/Math/MathUtils.hpp"

//////////////////////////////////////////////////////////////////////////
void ConvexHull2D::AddPlane(Vec2 const& normal, float distFromOrigin)
{
    m_boundingPlanes.emplace_back(normal,distFromOrigin);
}

//////////////////////////////////////////////////////////////////////////
bool ConvexHull2D::IsPointInside(Vec2 const& point) const
{
    for (Plane2D const& plane : m_boundingPlanes) {
        if (plane.IsPointInFront(point)) {
            return false;
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
bool ConvexHull2D::HitByRay(Vec2 const& start, Vec2 const& end, Vec2& outHitNormal, Vec2& outHitPoint, float& outLengthFrac) const
{
    //only detect hit in front face, return the farthest hit point
    float tempDistFraction = 0.f;
    bool hit = false;
    Vec2 tempNormal;
    Vec2 forward = end-start;
    for (Plane2D const& plane : m_boundingPlanes) {
        float seDot = DotProduct2D(plane.normal, forward);
        if (seDot >= 0.f) { //hit back of plane, ignore
            continue;
        }

        float sToPlaneDist = plane.GetDistance(start);
        if (-seDot < sToPlaneDist || sToPlaneDist<=0.f) { //SE in front of plane or S in hull, ignore
            continue;
        }

        float frac = -sToPlaneDist/seDot;
        if (frac > tempDistFraction) {
            tempDistFraction = frac;
            tempNormal = plane.normal;
            hit = true;
        }
    }
    if (hit) {
        Vec2 hitPoint = start + forward*tempDistFraction;
        if(IsPointInside(hitPoint)){
            outHitPoint = hitPoint;
            outHitNormal = tempNormal;
            outLengthFrac = tempDistFraction;
            return true;
        }
    }
    return false;
}
