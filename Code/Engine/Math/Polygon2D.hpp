#pragma once

#include <vector>
#include "Engine/Math/Vec2.hpp"

struct Disc2;
struct AABB2;
class ConvexHull2D;

class Polygon2D
{
public:
	//static constructors
	static Polygon2D MakeFromPointList( Vec2 const* points, unsigned int pointCount );
	static Polygon2D MakeFromPointCloud( Vec2 const* points, unsigned int pointCount );

	void Translate(Vec2 const& deltaPos);
	void RotateDegreesAroundPoint(float degrees, Vec2 const& point);
	void ScaleUniformlyAroundPoint(float scale, Vec2 const& point);

	bool IsValid() const; //points number >=3
	bool IsConvex() const;
	bool Contains( Vec2 const& point ) const;
	float GetDistance( Vec2 const& point ) const;
	float GetMomentInertiaAroundZ(float mass) const;
	ConvexHull2D GetConvexHull() const;
	AABB2 GetOutterBounds() const;
	Disc2 GetSmallestOuterDisc() const;
	Vec2 GetClosestPoint( Vec2 const& point ) const;
	Vec2 GetClosestPoint(Vec2 const& point, int& edgeIdx) const;
	Vec2 GetCentroid() const;
	Vec2 GetSupport(Vec2 const& direction) const;
	std::vector<Vec2> GetPoints() const { return m_points; }

	int GetVertexCount() const;
	int GetEdgeCount() const;
	void GetEdge( int idx, Vec2* outStart, Vec2* outEnd ) const;

private:
	std::vector<Vec2> m_points; //counter clock-wise
};