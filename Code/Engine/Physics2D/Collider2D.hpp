#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Delegate.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Physics2D/PhysicsMaterial.hpp"
#include <vector>

struct Vertex_PCU;
class Physics2D;
class Rigidbody2D;
struct Manifold2;
enum eSimulationMode : int;

//////////////////////////////////////////////////////////////////////////
enum eCollider2DType
{
	COLLIDER2D_DISC,
	COLLIDER2D_POLYGON,

	NUM_COLLIDER2D_TYPES
};

//////////////////////////////////////////////////////////////////////////
class Collider2D
{
	friend class Physics2D;

public:
	Delegate<Collider2D const*, Collider2D const*> OnTriggerEnter;
	Delegate<Collider2D const*, Collider2D const*> OnTriggerStay;
	Delegate<Collider2D const*, Collider2D const*> OnTriggerLeave;

public: // Interface 
	Collider2D() = default;
   // cache off the world shape representation of this object
   // taking into account the owning rigidbody (if no owner, local is world)
	virtual void UpdateWorldShape() = 0;
	virtual void Move(Vec2 const& moveDelta) = 0;

	// queries 
	virtual Disc2	GetWorldBounds() const = 0;
    virtual Vec2    GetSupport(Vec2 const& direction) const = 0;
	virtual Vec2	GetClosestPoint( Vec2 pos ) const = 0;
	virtual bool	Contains( Vec2 pos ) const = 0;
	virtual float	CalculateMomentInertia(float mass) const = 0;

	// debug helpers
	virtual void AddVertsForDebugRender( std::vector<Vertex_PCU>& verts, Rgba8 const& borderColor, Rgba8 const& fillColor, 
		bool drawBounds = false, float borderWidth = .2f, float scale=1.f ) = 0;

protected:
	virtual ~Collider2D(); // private - make sure this is virtual so correct deconstructor gets called

public: // any helpers you want to add
	eCollider2DType	GetType() const { return m_type; }
	bool			IsTrigger() const { return m_isTrigger; }
    bool			Intersects(Collider2D const* other) const;
    bool			GetManifold(Collider2D const* other, Manifold2* outManifold)const;
    float			GetBounceWith(Collider2D const* other) const;
	float			GetFrictionWith(Collider2D const* other) const;
    float			GetMass() const;
	eSimulationMode	GetSimulationMode() const;

	void			SetIsTrigger(bool isTrigger);

public:
	eCollider2DType m_type;                  // keep track of the type - will help with collision later
	Physics2D* m_system = nullptr;                   // system who created or destr
	Rigidbody2D* m_rigidbody = nullptr;    // owning rigidbody, used for calculating world shape
    PhysicsMaterial m_physicsMaterial;

private:
	bool m_isDestroyed = false;
	bool m_isTrigger = false;
};
