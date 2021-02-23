#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/ConvexHull2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//////////////////////////////////////////////////////////////////////////
Polygon2D Polygon2D::MakeFromPointList( Vec2 const* points, unsigned int pointCount )
{
	//TODO validation neglects "One Line" polygon
	Polygon2D* result = new Polygon2D();
	if( pointCount < 3 )
	{
		ERROR_RECOVERABLE( "Polygon constructs from less than 3 points" );
	}

	for( unsigned int idx = 0; idx < pointCount; idx++ )
	{
		if( result->m_points.size() > 2 ) //validate for convex
		{
			Vec2 start = result->m_points[0];
			Vec2 fromStart = result->m_points[1] - start;
			Vec2 toStartNormal = (start - points[idx]).GetRotated90Degrees();
			if( DotProduct2D( fromStart, toStartNormal ) < 0.f )
			{
				ERROR_RECOVERABLE( "Polygon would NOT be convex" );
			}
		}
		result->m_points.push_back( points[idx] );
	}

	return *result;
}

//////////////////////////////////////////////////////////////////////////
Polygon2D Polygon2D::MakeFromPointCloud( Vec2 const* points, unsigned int pointCount )
{
	if( pointCount < 3 )
	{
		return MakeFromPointList( points, pointCount );
	}

	//find right most point to start
	unsigned int rIdx = 0;
	for( unsigned int idx = 1; idx < pointCount; idx++ )
	{
		if( points[idx].x > points[rIdx].x ||
			(points[idx].x == points[rIdx].x && points[idx].y < points[rIdx].y) )
		{
			rIdx = idx;
		}
	}

	//gift wrapping algorithm
	Polygon2D* result = new Polygon2D();
	unsigned int pointOnHull = rIdx;
	do
	{
		Vec2 edgePoint = points[pointOnHull];
		result->m_points.push_back( edgePoint );
		unsigned int nextIdx = 0;
		for( unsigned int idx = 1; idx < pointCount; idx++ )
		{
			if( nextIdx == pointOnHull || IsPointOnRightSide(points[idx],edgePoint,points[nextIdx] ))
			{
				nextIdx = idx;
			}
		}
		pointOnHull = nextIdx;
	} while( pointOnHull != rIdx );

	return *result;
}

//////////////////////////////////////////////////////////////////////////
void Polygon2D::Translate(Vec2 const& deltaPos)
{
	for (size_t idx = 0; idx < m_points.size(); idx++)
	{
		m_points[idx] += deltaPos;
	}
}

//////////////////////////////////////////////////////////////////////////
void Polygon2D::RotateDegreesAroundPoint(float degrees, Vec2 const& point)
{
	for (Vec2& vec : m_points) {
		Vec2 delta = (vec-point).GetRotatedDegrees(degrees);
		vec = point+delta;
	}
}

//////////////////////////////////////////////////////////////////////////
void Polygon2D::ScaleUniformlyAroundPoint(float scale, Vec2 const& point)
{
    for (Vec2& vec : m_points) {
        Vec2 delta = (vec - point)*scale;
        vec = point + delta;
    }
}

//////////////////////////////////////////////////////////////////////////
bool Polygon2D::IsValid() const
{
	if( m_points.size() < 3 )
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Polygon2D::IsConvex() const
{
	if( !IsValid() )
	{
		return false;
	}

	size_t pointsNum = m_points.size();
	for( size_t startIdx = 0; startIdx < pointsNum - 2; startIdx++ )
	{
		Vec2 second = m_points[startIdx + 1];
		Vec2 normal = (second - m_points[startIdx]).GetRotated90Degrees();
		Vec2 nextEdge = m_points[startIdx + 2] - second;
		if( DotProduct2D( normal, nextEdge ) < 0.f )
		{
			return false;
		}
	}
    //deal with last case
    Vec2 start = m_points[0];
    Vec2 normal = (start - m_points[pointsNum - 1]).GetRotated90Degrees();
    Vec2 nextEdge = m_points[1] - start;
    if (DotProduct2D(normal, nextEdge) < 0.f)
    {
        return false;
    }
	//deal with first last case
	Vec2 last = m_points[pointsNum - 1];
	normal = (last - m_points[pointsNum - 2]).GetRotated90Degrees();
	nextEdge = m_points[0] - last;
	if( DotProduct2D( normal, nextEdge ) < 0.f )
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool Polygon2D::Contains( Vec2 const& point ) const
{
	//TODO assume already convex and valid
	size_t length = m_points.size();
	for( size_t idx = 0; idx < length - 1; idx++ )
	{
		if( IsPointOnRightSide( point, m_points[idx], m_points[idx + 1] ) )
		{
			return false;
		}
	}

	if (IsPointOnRightSide(point, m_points[length - 1], m_points[0]))
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
float Polygon2D::GetDistance( Vec2 const& point ) const
{
	Vec2 nearestPoint = GetClosestPoint( point );
	return GetDistance2D( nearestPoint, point );
}

//////////////////////////////////////////////////////////////////////////
float Polygon2D::GetMomentInertiaAroundZ(float mass) const
{
	//Assume center is zero
	size_t pointNum = m_points.size();
	
	float crossProductSum = 0.f;
	float TotalSum = 0.f;
	for (size_t i = 0; i < pointNum; i++){
		Vec2 first = m_points[i];
		size_t next = i + 1;
		next = next == pointNum ? 0 : next;
		Vec2 second = m_points[next];

		float crossProduct = AbsFloat(second.x * first.y - second.y * first.x);
		crossProductSum += crossProduct;
		TotalSum += crossProduct * (first.GetLengthSquared() + second.GetLengthSquared() + DotProduct2D(first, second));
	}
	return mass * TotalSum / (6.f * crossProductSum);
}

//////////////////////////////////////////////////////////////////////////
ConvexHull2D Polygon2D::GetConvexHull() const
{
	ConvexHull2D hull;
	for (size_t i = 0; i < m_points.size(); i++) {
		size_t nextIdx = i==m_points.size()-1?0:i+1;
		Vec2 normal = (m_points[nextIdx]-m_points[i]).GetNormalized().GetRotatedMinus90Degrees();
		float dist = DotProduct2D(m_points[i], normal);
		hull.AddPlane(normal, dist);
	}
	return hull;
}

//////////////////////////////////////////////////////////////////////////
AABB2 Polygon2D::GetOutterBounds() const
{
    float minX = m_points[0].x;
    float minY = m_points[0].y;
    float maxX = m_points[0].x;
    float maxY = m_points[0].y;
	for (Vec2 const& vec : m_points) {
		if (vec.x > maxX) {
			maxX = vec.x;
		}
		else if (vec.x < minX) {
			minX = vec.x;
		}
		if (vec.y > maxY) {
			maxY = vec.y;
		}
		else if (vec.y < minY) {
			minY = vec.y;
		}
	}
	return AABB2(minX, minY, maxX, maxY);
}

//////////////////////////////////////////////////////////////////////////
Disc2 Polygon2D::GetSmallestOuterDisc() const
{
	return GetMinimumOuterDiscForPolygon(&m_points[0], (int)m_points.size());
}

//////////////////////////////////////////////////////////////////////////
Vec2 Polygon2D::GetClosestPoint( Vec2 const& point ) const
{
	size_t pointsNum = m_points.size();
	//deal with last case first
	Vec2 result = GetNearestPointOnLineSegment2D( point, m_points[pointsNum - 1], m_points[0] );
	float shortestLengthSquared = GetDistanceSquared2D( point, result );
	for( size_t idx = 0; idx < pointsNum - 1; idx++ )
	{
		Vec2 tempResult = GetNearestPointOnLineSegment2D( point, m_points[idx], m_points[idx + 1] );
		float tempLengthSquared = GetDistanceSquared2D( point, tempResult );
		if( tempLengthSquared < shortestLengthSquared )
		{
			shortestLengthSquared = tempLengthSquared;
			result = tempResult;
		}
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////
Vec2 Polygon2D::GetClosestPoint(Vec2 const& point, int& edgeIdx) const
{
    size_t pointsNum = m_points.size();
    //deal with last case first
    Vec2 result = GetNearestPointOnLineSegment2D(point, m_points[pointsNum - 1], m_points[0]);
    float shortestLengthSquared = GetDistanceSquared2D(point, result);
	edgeIdx = (int)pointsNum-1;
    for (size_t idx = 0; idx < pointsNum - 1; idx++)
    {
        Vec2 tempResult = GetNearestPointOnLineSegment2D(point, m_points[idx], m_points[idx + 1]);
        float tempLengthSquared = GetDistanceSquared2D(point, tempResult);
        if (tempLengthSquared < shortestLengthSquared)
        {
			edgeIdx = (int)idx;
            shortestLengthSquared = tempLengthSquared;
            result = tempResult;
        }
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
Vec2 Polygon2D::GetCentroid() const
{
	return GetCentroidOfPolygon(&m_points[0], (int)m_points.size());
}

//////////////////////////////////////////////////////////////////////////
Vec2 Polygon2D::GetSupport(Vec2 const& direction) const
{
	float farestDistance = -1.f;
	size_t farestPointIndex = 0;

	for (size_t i = 0; i < m_points.size(); i++) {
		float distance = DotProduct2D(direction, m_points[i]);
		if (distance > farestDistance) {
			farestDistance = distance;
			farestPointIndex = i;
		}
	}

	return m_points[farestPointIndex];
}

//////////////////////////////////////////////////////////////////////////
int Polygon2D::GetVertexCount() const
{
	return (int)m_points.size();
}

//////////////////////////////////////////////////////////////////////////
int Polygon2D::GetEdgeCount() const
{
	return (int)m_points.size();
}

//////////////////////////////////////////////////////////////////////////
void Polygon2D::GetEdge( int idx, Vec2* outStart, Vec2* outEnd ) const
{
	if( idx < 0 || idx >= m_points.size() )
	{
		outStart = nullptr;
		outEnd = nullptr;
	}
	else
	{
		*outStart = m_points[idx];
		
		int nextIdx = idx + 1;
		if( nextIdx == m_points.size() )
		{
			nextIdx = 0;
		}
		*outEnd = m_points[nextIdx];
	}
}
