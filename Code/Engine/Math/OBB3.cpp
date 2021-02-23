#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB3.hpp"

//////////////////////////////////////////////////////////////////////////
OBB3::OBB3(AABB3 const& aabb)
{
    center = aabb.GetCenter();
    halfDimensions = aabb.GetDimensions()*.5f;
}

//////////////////////////////////////////////////////////////////////////
OBB3::OBB3(Vec3 const& c, Vec3 const& hDim, Vec3 const& rightDir /*= Vec3(1.f,0.f,0.f)*/, Vec3 const& upDir /*= Vec3(0.f,1.f,0.f)*/)
    :center(c)
    ,halfDimensions(hDim)
    ,right(rightDir)
    ,up(upDir)
{
}

//////////////////////////////////////////////////////////////////////////
Vec3 OBB3::GetForwardVector() const
{
    return CrossProduct3D(right, up);
}
