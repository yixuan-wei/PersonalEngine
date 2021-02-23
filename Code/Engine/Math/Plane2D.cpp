#include "Engine/Math/Plane2D.hpp"
#include "Engine/Math/MathUtils.hpp"

//////////////////////////////////////////////////////////////////////////
Plane2D::Plane2D(Vec2 const& n, Vec2 const& pointOnPlane)
	:normal(n)
	,distanceFromOriginAlongNormal(DotProduct2D(n,pointOnPlane))
{
}

//////////////////////////////////////////////////////////////////////////
Plane2D::Plane2D(Vec2 const& n, float distFromOrigin)
	: normal(n)
	, distanceFromOriginAlongNormal(distFromOrigin)
{
}

//////////////////////////////////////////////////////////////////////////
float Plane2D::GetDistance( Vec2 const& point ) const
{
	return DotProduct2D(point,normal)-distanceFromOriginAlongNormal;
}

//////////////////////////////////////////////////////////////////////////
bool Plane2D::IsPointInFront(Vec2 const& point) const
{
	static float delta = AbsFloat(distanceFromOriginAlongNormal*.00001f);
	return GetDistance(point) > delta;
}
