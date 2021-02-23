#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Delegate.hpp"
#include "Engine/Core/NamedProperties.hpp"

class Collider2D;
class Physics2D;
struct Collision2D;

enum eSimulationMode 
{
	STATIC,
	KINEMATIC,
	DYNAMIC
};

//////////////////////////////////////////////////////////////////////////
class Rigidbody2D
{
	friend class Physics2D;

public:
	Delegate<Collision2D const&> onOverlapStart;
	Delegate<Collision2D const&> onOverlapStay;
	Delegate<Collision2D const&> onOverlapStop;
	Delegate<Collision2D const&> onCollision;	//called after collision resolved

public:
	void Destroy(); // helper for destroying myself (uses owner to destroy self)
	void TakeCollider(Collider2D* collider); // takes ownership of a collider (destroying my current one if present)
	void BeginSimulation();

	void Simulate(float deltaSeconds);
	void ApplyForce(Vec2 const& newForce);
	void ApplyGravityForce(Vec2 const& gravityAcceleration);
	void ApplyDragForce();
	void ApplyImpulseAt(Vec2 const& impulse, Vec2 const& pos);

	void SetLayer(unsigned int layerIndex);
	unsigned int GetLayer() const { return m_layerIndex; }

	void SetRotationLocked(bool isLocked);
	void SetXAxisLocked(bool isLocked);
	void SetYAxisLocked(bool isLocked);
	void SetPosition( Vec2 newPosition );
	void Move(Vec2 const& deltaMove);
	void SetEnable(bool isEnabled);
	void SetVelocity(Vec2 const& velocity);
	void SetSimulationMode(eSimulationMode mode);
	void SetBounciness(float newBounciness);
	void SetFriction(float newFriction);
	void SetDrag(float newDrag);
	void SetMass(float newMass);
	void SetMomentInertia(float newMomentInertia);
	void SetAngularVelocity(float newAngularVelocityDegrees);
	void SetRotationInRadius(float newRotationDegrees);

	//Calculate const
	float CalculateMomentInertia();

	//Getters
	Vec2			GetVerletVelocity() const;
	Vec2			GetImpactVelocity(Vec2 const& contactPos) const;
	float			GetMass() const;
    float			GetBounciness()const;
    float			GetFriction() const;
    float			GetRotationDegrees() const;
	float			GetAngularVelocityDegrees() const;
	float			GetMomentInertia() const;
	float			GetRotationRadians() const  { return m_rotationInRadius; }
    float			GetDrag() const				{ return m_drag; }	
	Collider2D*		GetCollider() const			{ return m_collider; }
	Vec2			GetWorldPosition() const	{ return m_worldPosition; }
	Vec2			GetVelocity() const			{ return m_velocity; }
	eSimulationMode	GetSimulationMode() const	{ return m_mode; }
	bool			IsRotationLocked() const	{ return m_rotationLocked; }
	bool			IsXAxisLocked() const		{ return m_xAxisLocked; }
	bool			IsYAxisLocked() const		{ return m_yAxisLocked; }
	bool			IsDestroyed() const			{ return m_isDestroyed; }
	bool			IsEnabled() const			{ return m_isEnabled; }

public:
	Physics2D* m_system = nullptr;     // which scene created/owns this object
	Collider2D* m_collider = nullptr;

	Vec2 m_worldPosition;   // where in the world is this rigidbody
	NamedProperties m_userProperties;

private:
	float m_drag = 0.f;
	float m_mass = 1.f;
	float m_massInverse = 1.f;

	float m_rotationInRadius = 0.f; //Radians
	float m_angularVelocity = 0.f;
	float m_frameTorque = 0.f;
	float m_momentInertia = 0.f;

	Vec2 m_frameForce;
	Vec2 m_velocity;
	eSimulationMode m_mode = DYNAMIC;
	unsigned int m_layerIndex = 0;

	Vec2 m_lastWorldPos;

	bool m_rotationLocked = false;
	bool m_xAxisLocked = false;
	bool m_yAxisLocked = false;
	bool m_isEnabled = true;
	bool m_isDestroyed = false;

private:
	~Rigidbody2D(); // destroys the collider
};