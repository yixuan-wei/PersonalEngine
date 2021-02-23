#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/MathUtils.hpp"

//////////////////////////////////////////////////////////////////////////
LineSegment2::LineSegment2( const Vec2& start, const Vec2& end )
	:start(start)
	,end(end)
{
}

//////////////////////////////////////////////////////////////////////////
const Vec2 LineSegment2::GetCenter() const
{
	return (start + end) * .5f;
}

//////////////////////////////////////////////////////////////////////////
const Vec2 LineSegment2::GetNearestPoint(const Vec2& point) const
{
	Vec2 startToEnd = end - start;
	float segmentLengthSquared = startToEnd.GetLengthSquared();
	if( segmentLengthSquared == 0.f ) //line segment is a point
	{
		return start;
	}

	Vec2 startToRefPos = point - start;
	float projectedUnit = DotProduct2D( startToEnd, startToRefPos ) / segmentLengthSquared;
	Vec2 startToRefProjected = projectedUnit * startToEnd;

	if( projectedUnit <= 0.f ) // nearest point is start
	{
		return start;
	}
	else if( projectedUnit >= 1.f )//nearest point is end
	{
		return end;
	}
	else
	{
		return start + startToEnd * projectedUnit;
	}
}

//////////////////////////////////////////////////////////////////////////
float LineSegment2::GetLength() const
{
	return GetDistance2D(start, end);
}

/////////////////////////////////////////////////////////////////////////
float LineSegment2::GetLengthSquared() const
{
	return GetDistanceSquared2D(start, end);
}

//////////////////////////////////////////////////////////////////////////
bool LineSegment2::IsSegmentPoint() const
{
	return start == end;
}

//////////////////////////////////////////////////////////////////////////
void LineSegment2::Translate( const Vec2& translation)
{
	start += translation;
	end += translation;
}

//////////////////////////////////////////////////////////////////////////
void LineSegment2::RotateDegreesAboutCenter( float deltaDegrees )
{
	Vec2 center = (end + start) * .5f;
	Vec2 centerToStart = start - center;
	centerToStart.RotateDegrees( deltaDegrees );
	start = centerToStart + center;
	end = center - centerToStart;
}
