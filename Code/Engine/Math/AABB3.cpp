#include "Engine/Math/AABB3.hpp"

//////////////////////////////////////////////////////////////////////////
AABB3::AABB3(AABB3 const& copyFrom)
    :mins(copyFrom.mins)
    ,maxs(copyFrom.maxs)
{
}

//////////////////////////////////////////////////////////////////////////
AABB3::AABB3(Vec3 const& inMins, Vec3 const& inMaxs)
    :mins(inMins)
    ,maxs(inMaxs)
{
}

//////////////////////////////////////////////////////////////////////////
bool AABB3::IsPointInside(Vec3 const& point) const
{
    if (point.x<maxs.x && point.x>mins.x&&
        point.y<maxs.y && point.y>mins.y&&
        point.z<maxs.z && point.z>mins.z) {
        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
Vec3 AABB3::GetCenter() const
{
    return (mins + maxs) * .5f;
}

//////////////////////////////////////////////////////////////////////////
Vec3 AABB3::GetDimensions() const
{
    return maxs - mins;
}

//////////////////////////////////////////////////////////////////////////
float AABB3::GetOutterRadius() const
{
    Vec3 radius = maxs - GetCenter();
    return radius.GetLength();
}

//////////////////////////////////////////////////////////////////////////
//order: bottom counter clockwise from mins, then top ccw from (mins.x, maxs.y, mins.z)
void AABB3::GetCornerPoints(Vec3* outEightPoints) const
{
    outEightPoints[0] = mins;
    outEightPoints[1] = Vec3(maxs.x, mins.y, mins.z);
    outEightPoints[2] = Vec3(maxs.x, mins.y, maxs.z);
    outEightPoints[3] = Vec3(mins.x, mins.y, maxs.z);
    outEightPoints[4] = Vec3(mins.x, maxs.y, mins.z);
    outEightPoints[5] = Vec3(maxs.x, maxs.y, mins.z);
    outEightPoints[6] = maxs;
    outEightPoints[7] = Vec3(mins.x, maxs.y, maxs.z);
}

//////////////////////////////////////////////////////////////////////////
void AABB3::Translate(const Vec3& translation)
{
    mins += translation;
    maxs += translation;
}

//////////////////////////////////////////////////////////////////////////
void AABB3::SetCenter(const Vec3& newCenter)
{
    Vec3 oldCenter = GetCenter();
    Vec3 translation = newCenter - oldCenter;
    Translate(translation);
}

//////////////////////////////////////////////////////////////////////////
void AABB3::operator=(AABB3 const& assignFrom)
{
    mins = assignFrom.mins;
    maxs = assignFrom.maxs;
}

//////////////////////////////////////////////////////////////////////////
bool AABB3::operator==(AABB3 const& compareWith) const
{
    return (mins == compareWith.mins) && (maxs == compareWith.maxs);
}
