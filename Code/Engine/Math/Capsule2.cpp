#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/MathUtils.hpp"

//////////////////////////////////////////////////////////////////////////
Capsule2::Capsule2( const Vec2& startPos, const Vec2& endPos, float capsuleRadius )
	:boneStart(startPos)
	,boneEnd(endPos)
	,radius(capsuleRadius)
{
}

//////////////////////////////////////////////////////////////////////////
bool Capsule2::IsPointInside( const Vec2& point ) const
{
	Vec2 startToEnd = boneEnd-boneStart;
	Vec2 startToRefPos = point - boneStart;
	float segmentLengthSquared = startToEnd.GetLengthSquared();
	if( segmentLengthSquared == 0.f ) //line segment is a point
	{
		if( startToRefPos.GetLengthSquared() < radius*radius )
			return true;
		else
			return false;
	}

	float projectedUnit = DotProduct2D( startToEnd, startToRefPos ) / segmentLengthSquared;
	Vec2 startToRefProjected = projectedUnit * startToEnd;
	if( projectedUnit <= 0.f ) // nearest point is start semi-circle
	{
		if( startToRefPos.GetLengthSquared() < radius*radius )
			return true;
		else
			return false;
	}
	else if( projectedUnit >= 1.f )//nearest point is end semi-circle
	{
		Vec2 endToRefPos = point - boneEnd;
		if( endToRefPos.GetLengthSquared() < radius*radius )
			return true;
		else
			return false;
	}
	else
	{
		Vec2 pointOnLine = boneStart + startToEnd * projectedUnit;
		Vec2 onLineToRef = (point - pointOnLine);
		if( onLineToRef.GetLengthSquared() < radius*radius )
			return true;
		else
			return false;
	}
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Capsule2::GetNearestPoint(const Vec2& point) const
{
	Vec2 startToEnd = boneEnd-boneStart;
	Vec2 startToRefPos = point - boneStart;
	float segmentLengthSquared = startToEnd.GetLengthSquared();
	if( segmentLengthSquared == 0.f ) //line segment is a point
	{
		return startToRefPos.GetClamped( radius ) + boneStart;
	}

	float projectedUnit = DotProduct2D( startToEnd, startToRefPos ) / segmentLengthSquared;
	Vec2 startToRefProjected = projectedUnit * startToEnd;

	if( projectedUnit <= 0.f ) // nearest point is start semi-circle
	{
		return startToRefPos.GetClamped( radius ) + boneStart;
	}
	else if( projectedUnit >= 1.f )//nearest point is end semi-circle
	{
		Vec2 endToRefPos = point - boneEnd;
		return boneEnd + endToRefPos.GetClamped( radius );
	}
	else
	{
		Vec2 pointOnLine = boneStart + startToEnd * projectedUnit;
		Vec2 onLineToBound = (point - pointOnLine).GetClamped( radius );
		return pointOnLine + onLineToBound;
	}
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Capsule2::GetCenter() const
{
	return (boneStart + boneEnd) * .5f;
}

//////////////////////////////////////////////////////////////////////////
void Capsule2::Translate(const Vec2& translation)
{
	boneStart += translation;
	boneEnd += translation;
}

//////////////////////////////////////////////////////////////////////////
void Capsule2::RotateDegreesAboutCenter(float deltaDegrees)
{
	Vec2 center = (boneEnd + boneStart) * .5f;
	Vec2 centerToStart = boneStart - center;
	centerToStart.RotateDegrees( deltaDegrees );
	boneStart = centerToStart + center;
	boneEnd = center - centerToStart;
}
