#pragma once

#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Math/Polygon2D.hpp"

class Polygon2D;
struct LineSegment2;

class PolygonCollider2D :public Collider2D
{
public:
	PolygonCollider2D(Vec2 const* points, unsigned int pointCount);
	explicit PolygonCollider2D(Polygon2D const& polygon);

	virtual void UpdateWorldShape() override;
	virtual void Move(Vec2 const& moveDelta) override;

	virtual Disc2 GetWorldBounds() const override;
	virtual Vec2 GetSupport(Vec2 const& direction) const override;
	virtual Vec2 GetClosestPoint( Vec2 pos ) const override;
	virtual bool Contains( Vec2 pos ) const override;
	virtual float CalculateMomentInertia(float mass) const override;

	virtual void AddVertsForDebugRender(std::vector<Vertex_PCU>& verts, Rgba8 const& borderColor, Rgba8 const& fillColor, 
		bool drawBounds = false, float borderWidth=.2f, float scale = 1.f )override;

public:
	void UpdateBounds();

	void GetPoints(std::vector<Vec2>& points) const;
    Vec2 GetClosestPoint(Vec2 const& pos, int& edgeIdx) const;
	float GetBoundRaiuds() const { return m_bounds.radius; }
	LineSegment2 GetContactEdge(Vec2 const& direction, float epsilon) const;

protected:
	virtual ~PolygonCollider2D() override;

private:
	Vec2 m_worldPosition;
	Disc2 m_bounds;

public:
	Polygon2D m_polygon;
};