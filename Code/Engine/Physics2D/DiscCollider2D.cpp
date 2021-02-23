#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//////////////////////////////////////////////////////////////////////////
DiscCollider2D::DiscCollider2D( Vec2 const& localPos, float radius )
	:Collider2D()
	,m_disc(localPos,radius)
{
	m_type = eCollider2DType::COLLIDER2D_DISC;
}

//////////////////////////////////////////////////////////////////////////
void DiscCollider2D::UpdateWorldShape()
{
	m_worldPosition = m_rigidbody->GetWorldPosition() + m_disc.center;
}

//////////////////////////////////////////////////////////////////////////
void DiscCollider2D::Move(Vec2 const& moveDelta)
{
	m_worldPosition += moveDelta;
}

//////////////////////////////////////////////////////////////////////////
Disc2 DiscCollider2D::GetWorldBounds() const
{
    Mat44 rotate = Mat44::CreateZRotationDegrees(m_rigidbody->GetRotationDegrees());
    Vec2 newLocalCenter = rotate.TransformPosition2D(m_disc.center);
	return Disc2(newLocalCenter + m_worldPosition, m_disc.radius);
}

//////////////////////////////////////////////////////////////////////////
Vec2 DiscCollider2D::GetSupport(Vec2 const& direction) const
{
	return m_worldPosition + m_disc.center + direction.GetNormalized() * m_disc.radius;
}

//////////////////////////////////////////////////////////////////////////
Vec2 DiscCollider2D::GetClosestPoint( Vec2 pos ) const
{
    Mat44 negaRotate = Mat44::CreateZRotationDegrees(-m_rigidbody->GetRotationDegrees());
    Vec2 newLocalPos = negaRotate.TransformVector2D(pos-m_worldPosition-m_disc.center);
	return m_disc.GetNearestPoint(newLocalPos) + m_worldPosition + m_disc.center;
}

//////////////////////////////////////////////////////////////////////////
bool DiscCollider2D::Contains( Vec2 pos ) const
{
    Mat44 negaRotate = Mat44::CreateZRotationDegrees(-m_rigidbody->GetRotationDegrees());
    Vec2 newLocalPos = negaRotate.TransformVector2D(pos - m_worldPosition-m_disc.center);
	return m_disc.IsPointInside(newLocalPos);
}

//////////////////////////////////////////////////////////////////////////
float DiscCollider2D::CalculateMomentInertia(float mass) const
{
	float radius = m_disc.radius;
	float inertia = .5f * mass;
	float parallel = fPI * m_disc.center.GetLengthSquared();
	return (inertia + parallel) * radius * radius;
}

//////////////////////////////////////////////////////////////////////////
void DiscCollider2D::AddVertsForDebugRender( std::vector<Vertex_PCU>& verts, Rgba8 const& borderColor, 
	Rgba8 const& fillColor, bool drawBounds, float borderWidth, float scale )
{
	UNUSED(drawBounds);

	std::vector<Vertex_PCU> tempVerts;
	//draw disc
	AppendVertsForDisc2D(tempVerts, m_disc.center, m_disc.radius, fillColor);
	AppendVertsForRing2D(tempVerts, m_disc.center, m_disc.radius, borderWidth, borderColor);
	if (drawBounds) {
	//draw center
		Vec2 vertical = Vec2(1.f, 1.f) * borderWidth;
		Vec2 horizontal(-vertical.y, vertical.x);
		Rgba8 centerColor = Rgba8::BLUE;
		if (!m_rigidbody->IsEnabled())
		{
			centerColor = Rgba8::RED;
		}
		AppendVertsForLine2D(tempVerts, m_disc.center + vertical, m_disc.center - vertical, borderWidth, centerColor);
		AppendVertsForLine2D(tempVerts, m_disc.center + horizontal, m_disc.center - horizontal, borderWidth, centerColor);
		//draw 0 degree radius
		AppendVertsForLine2D(tempVerts, m_disc.center, m_disc.center + Vec2(m_disc.radius, 0.f), borderWidth, borderColor);
	}
	TransformVertexArray((int)tempVerts.size(), &tempVerts[0], scale, m_rigidbody->GetRotationDegrees(), m_worldPosition);

	verts.insert(verts.end(), tempVerts.begin(), tempVerts.end());
}
