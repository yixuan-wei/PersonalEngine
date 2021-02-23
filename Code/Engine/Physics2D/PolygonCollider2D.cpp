#include "Engine/Physics2D/PolygonCollider2D.hpp"
#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//////////////////////////////////////////////////////////////////////////
PolygonCollider2D::PolygonCollider2D( Vec2 const* points, unsigned int pointCount)
{
	m_type = eCollider2DType::COLLIDER2D_POLYGON;
	m_polygon = Polygon2D::MakeFromPointList( points, pointCount );
	m_bounds = m_polygon.GetSmallestOuterDisc();
	GUARANTEE_OR_DIE( m_polygon.IsConvex(), "PolygonCollider2D constructs non Convex" );
}

//////////////////////////////////////////////////////////////////////////
PolygonCollider2D::PolygonCollider2D(Polygon2D const& polygon)
{
	m_type = eCollider2DType::COLLIDER2D_POLYGON;
	m_polygon = polygon;
	m_bounds = m_polygon.GetSmallestOuterDisc();
	GUARANTEE_OR_DIE(m_polygon.IsConvex(), "PolygonCollider2D constructs non Convex");
}

//////////////////////////////////////////////////////////////////////////
void PolygonCollider2D::UpdateWorldShape()
{
	m_worldPosition = m_rigidbody->GetWorldPosition();
}

//////////////////////////////////////////////////////////////////////////
void PolygonCollider2D::Move(Vec2 const& moveDelta)
{
	m_worldPosition += moveDelta;
}

//////////////////////////////////////////////////////////////////////////
Disc2 PolygonCollider2D::GetWorldBounds() const
{
	Vec2 newLocalCenter = m_bounds.center.GetRotatedRadians(m_rigidbody->GetRotationRadians());
	return Disc2(newLocalCenter+m_worldPosition,m_bounds.radius);
}

//////////////////////////////////////////////////////////////////////////
Vec2 PolygonCollider2D::GetSupport(Vec2 const& direction) const
{
	float rotationRadians = m_rigidbody->GetRotationRadians();
	Vec2 local = m_polygon.GetSupport(direction.GetRotatedRadians(-rotationRadians));
	local.RotateRadians(rotationRadians);
	return local+m_worldPosition;
}

//////////////////////////////////////////////////////////////////////////
Vec2 PolygonCollider2D::GetClosestPoint( Vec2 pos ) const
{
	float rotationRadians = m_rigidbody->GetRotationRadians();
	Vec2 localPos = pos - m_worldPosition;
	localPos.RotateRadians(-rotationRadians);
	Vec2 closestLocal = m_polygon.GetClosestPoint( localPos );
	closestLocal.RotateRadians(rotationRadians);
	return closestLocal + m_worldPosition;
}

//////////////////////////////////////////////////////////////////////////
Vec2 PolygonCollider2D::GetClosestPoint(Vec2 const& pos, int& edgeIdx) const
{
	float rotationRadians = m_rigidbody->GetRotationRadians();
	Vec2 localPos = pos - m_worldPosition;
	localPos.RotateRadians(-rotationRadians);
	Vec2 closestLocal = m_polygon.GetClosestPoint(localPos, edgeIdx);
	closestLocal.RotateRadians(rotationRadians);
	return closestLocal + m_worldPosition;
}

//////////////////////////////////////////////////////////////////////////
LineSegment2 PolygonCollider2D::GetContactEdge(Vec2 const& direction, float epsilon) const
{
	std::vector<Vec2> supports;

    float farestDistance = -1.f;
    size_t farestPointIndex = 0;
	std::vector<Vec2> points = m_polygon.GetPoints();
	//float newEpsilon = m_bounds.radius * direction.GetLength()*.03f;
	float radians = m_rigidbody->GetRotationRadians();
	Vec2 localDir = direction.GetRotatedRadians(-radians).GetNormalized();
	//find all point inside penetration
    for (size_t i = 0; i < points.size(); i++) {
        float distance = DotProduct2D(localDir, points[i]);
        if (NearlyEqual(distance, farestDistance, epsilon))
        {
            supports.push_back(points[i].GetRotatedRadians(radians) + m_worldPosition);
        }
        else if (distance > farestDistance) {
        farestDistance = distance;
        farestPointIndex = i;
        supports.clear();
        supports.push_back(points[i].GetRotatedRadians(radians) + m_worldPosition);
        }
    }

	if (supports.size() < 2) {//single point found in penetration
		size_t prevIndex = farestPointIndex==0?points.size()-1: farestPointIndex - 1;
		float distance0 = DotProduct2D(localDir, points[prevIndex]);
		size_t nextIndex = farestPointIndex == points.size() - 1 ? 0 : farestPointIndex + 1;
		float distance1 = DotProduct2D(localDir, points[nextIndex]);
		float maxi = MaxFloat(distance0, distance1);
		if (NearlyEqual(maxi, farestDistance, m_bounds.radius*.01f)) {
            if (distance0 > distance1) {
                return LineSegment2(points[prevIndex].GetRotatedRadians(radians) + m_worldPosition,
                    points[farestPointIndex].GetRotatedRadians(radians) + m_worldPosition);
            }
            else {
                return LineSegment2(points[farestPointIndex].GetRotatedRadians(radians) + m_worldPosition,
                    points[nextIndex].GetRotatedRadians(radians) + m_worldPosition);
            }
		}		
	}
	//multiple points found in penetration or single failed 
	return LineSegment2(supports[0], supports.back());
}

//////////////////////////////////////////////////////////////////////////
bool PolygonCollider2D::Contains( Vec2 pos ) const
{
	Vec2 localPos = pos - m_worldPosition;
    localPos.RotateRadians(-m_rigidbody->GetRotationRadians());
	return m_polygon.Contains( localPos );
}

//////////////////////////////////////////////////////////////////////////
float PolygonCollider2D::CalculateMomentInertia(float mass) const
{
	return m_polygon.GetMomentInertiaAroundZ(mass);
}

//////////////////////////////////////////////////////////////////////////
void PolygonCollider2D::AddVertsForDebugRender( std::vector<Vertex_PCU>& verts, Rgba8 const& borderColor, 
	Rgba8 const& fillColor, bool drawBounds, float borderWidth, float scale )
{
	int edgeCount = m_polygon.GetEdgeCount();
	Vec2 first, second;
	m_polygon.GetEdge( 0, &first, &second );

	std::vector<Vertex_PCU> tempVerts;
    //draw bounds
    if (drawBounds) {
        AppendVertsForDisc2D(tempVerts, m_bounds.center, m_bounds.radius, Rgba8(0, 0, 255, 50));
    }
	//draw polygon
	AppendVertsForPolygon2D( tempVerts, m_polygon, fillColor );
	for( int idx = 0; idx < edgeCount; idx++ )
	{
		Vec2 start, end;
		m_polygon.GetEdge( idx, &start, &end );
		AppendVertsForLine2D( tempVerts, start, end, borderWidth, borderColor );
	}
	if (drawBounds) {
	//draw center
		Vec2 vertical = Vec2(1.f, 1.f) * borderWidth;
		Vec2 horizontal(-vertical.y, vertical.x);
		Rgba8 centerColor = Rgba8::BLUE;
		if (!m_rigidbody->IsEnabled())
		{
			centerColor = Rgba8::RED;
		}
		AppendVertsForLine2D(tempVerts, vertical, -vertical, borderWidth, centerColor);
		AppendVertsForLine2D(tempVerts, horizontal, -horizontal, borderWidth, centerColor);
	}
	TransformVertexArray((int)tempVerts.size(), &tempVerts[0], scale, m_rigidbody->GetRotationDegrees(), m_worldPosition);
	
	verts.insert(verts.end(), tempVerts.begin(), tempVerts.end());
}

//////////////////////////////////////////////////////////////////////////
void PolygonCollider2D::UpdateBounds()
{
	m_bounds = m_polygon.GetSmallestOuterDisc();
}

//////////////////////////////////////////////////////////////////////////
void PolygonCollider2D::GetPoints(std::vector<Vec2>& points) const
{
	points.clear();
	float radians = m_rigidbody->GetRotationRadians();
	size_t edgeCount = m_polygon.GetEdgeCount();
	for (size_t i = 0; i < edgeCount;) {
		Vec2 point1, point2;
		m_polygon.GetEdge((int)i, &point1, &point2);
		point1.RotateRadians(radians);
		points.push_back(point1 + m_worldPosition);
		if (i != edgeCount - 1) {
			point2.RotateRadians(radians);
			points.push_back(point2 + m_worldPosition);
		}
		i += 2;
	}
}

//////////////////////////////////////////////////////////////////////////
PolygonCollider2D::~PolygonCollider2D()
{
}
