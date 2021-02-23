#pragma once

#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Math/Disc2.hpp"

class DiscCollider2D : public Collider2D
{
public: // implement the interface of Collider2D
  //
	DiscCollider2D(Vec2 const& localPos, float radius);

	virtual void UpdateWorldShape() override;
	virtual void Move(Vec2 const& moveDelta)override;

	virtual Disc2 GetWorldBounds() const override;
	virtual Vec2 GetSupport(Vec2 const& direction) const override;
	virtual Vec2 GetClosestPoint( Vec2 pos ) const override;
	virtual bool Contains(Vec2 pos) const override;
	virtual float CalculateMomentInertia(float mass) const override;

	virtual void AddVertsForDebugRender( std::vector<Vertex_PCU>& verts, Rgba8 const& borderColor, Rgba8 const& fillColor, 
		bool drawBounds = false, float borderWidth = .2f, float scale=1.f )override;

protected:
	virtual ~DiscCollider2D() override {}

public:
	Disc2 m_disc;
	//Vec2 m_localPosition; // my local offset from my parent
	Vec2 m_worldPosition; // calculated from local position and owning rigidbody if present
	//float m_radius;
};