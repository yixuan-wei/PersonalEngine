#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Physics2D/Physics2D.hpp"
#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::Destroy()
{
	m_system->DestroyRigidbody( this );
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::Simulate(float deltaSeconds)
{
	if (m_mode != KINEMATIC)
	{
        Vec2 acceleration = m_frameForce * m_massInverse;
        m_velocity += acceleration * deltaSeconds;

		float angularAcceleration = m_frameTorque / GetMomentInertia();
		m_angularVelocity += angularAcceleration * deltaSeconds;
	}
	Move( m_velocity * deltaSeconds);
	m_rotationInRadius += m_angularVelocity * deltaSeconds;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::ApplyForce(Vec2 const& newForce)
{
	m_frameForce += newForce;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::ApplyGravityForce(Vec2 const& gravityAcceleration)
{
	Vec2 gravityForce = m_mass * gravityAcceleration;
	ApplyForce(gravityForce);
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::ApplyDragForce()
{
    Vec2 velocity = GetVelocity(); // GetVerletVelocity
    Vec2 dragForce = -velocity * m_drag;
    ApplyForce(dragForce);
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::ApplyImpulseAt(Vec2 const& impulse, Vec2 const& pos)
{
	m_velocity += impulse * m_massInverse;
	m_angularVelocity += DotProduct2D((pos - m_worldPosition).GetRotated90Degrees(), impulse) / GetMomentInertia();
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetLayer(unsigned int layerIndex)
{
	m_layerIndex = layerIndex;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetRotationLocked(bool isLocked)
{
	m_rotationLocked = isLocked;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetXAxisLocked(bool isLocked)
{
	m_xAxisLocked = isLocked;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetYAxisLocked(bool isLocked)
{
	m_yAxisLocked = isLocked;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::TakeCollider( Collider2D* collider )
{
	if( m_collider )
	{
		m_system->DestroyCollider( m_collider );
	}

	m_collider = collider;
	m_momentInertia = CalculateMomentInertia();
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::BeginSimulation()
{
	m_lastWorldPos = m_worldPosition;
    m_frameForce = Vec2::ZERO;
    m_frameTorque = 0.f;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetPosition( Vec2 newPosition )
{
	m_worldPosition = newPosition;
	m_collider->UpdateWorldShape();
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::Move(Vec2 const& deltaMove)
{
	Vec2 actualMove = deltaMove;
	if (m_xAxisLocked) {
		actualMove.x = 0.f;
	}
	if (m_yAxisLocked) {
		actualMove.y = 0.f;
	}
	m_worldPosition += actualMove;
	m_collider->UpdateWorldShape();
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetEnable(bool isEnabled)
{
	m_isEnabled = isEnabled;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetVelocity(Vec2 const& velocity)
{
	m_velocity = velocity;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetSimulationMode(eSimulationMode mode)
{
	m_mode = mode;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetBounciness(float newBounciness)
{
	newBounciness = Clamp(newBounciness, 0.f, 1.f);
	m_collider->m_physicsMaterial.bounciness = newBounciness;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetFriction(float newFriction)
{
	newFriction = Clamp(newFriction, 0.f, 1.f);
	m_collider->m_physicsMaterial.friction = newFriction;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetDrag(float newDrag)
{
	m_drag = m_drag < 0.f ? 0.f : m_drag;
	m_drag = newDrag;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetMass(float newMass)
{
	if (newMass == 0.f) {
		g_theConsole->PrintString(Rgba8::RED,"Setting rigid body 2d mass to zero");
		return;
	}

	m_momentInertia *= newMass / m_mass;
	m_mass = newMass;
	m_massInverse = 1.f / newMass;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetMomentInertia(float newMomentInertia)
{
	m_momentInertia = newMomentInertia;
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetAngularVelocity(float newAngularVelocityDegrees)
{
	m_angularVelocity = ConvertDegreesToRadians(newAngularVelocityDegrees);
}

//////////////////////////////////////////////////////////////////////////
void Rigidbody2D::SetRotationInRadius(float newRotationDegrees)
{
	m_rotationInRadius = ConvertDegreesToRadians(newRotationDegrees);
}

//////////////////////////////////////////////////////////////////////////
float Rigidbody2D::CalculateMomentInertia()
{
	if (m_collider != nullptr) {
		return m_collider->CalculateMomentInertia(m_mass);
	}
	else {
		return 0.f;
	}
}

//////////////////////////////////////////////////////////////////////////
Vec2 Rigidbody2D::GetVerletVelocity() const
{
	return (m_worldPosition - m_lastWorldPos) / (float)m_system->GetFixedDeltaTime();
}

//////////////////////////////////////////////////////////////////////////
Vec2 Rigidbody2D::GetImpactVelocity(Vec2 const& contactPos) const
{
	Vec2 centerToP = contactPos - m_worldPosition;
	Vec2 angularVel = m_angularVelocity * centerToP.GetRotated90Degrees();
	return m_velocity + angularVel;
}

//////////////////////////////////////////////////////////////////////////
float Rigidbody2D::GetMass() const
{
	return m_mass;
}

//////////////////////////////////////////////////////////////////////////
float Rigidbody2D::GetBounciness() const
{
	return m_collider->m_physicsMaterial.bounciness;
}

//////////////////////////////////////////////////////////////////////////
float Rigidbody2D::GetFriction() const
{
	return m_collider->m_physicsMaterial.friction;
}

//////////////////////////////////////////////////////////////////////////
float Rigidbody2D::GetRotationDegrees() const
{
	return ConvertRadiansToDegrees(m_rotationInRadius);
}

//////////////////////////////////////////////////////////////////////////
float Rigidbody2D::GetAngularVelocityDegrees() const
{
	return ConvertRadiansToDegrees(m_angularVelocity);
}

//////////////////////////////////////////////////////////////////////////
float Rigidbody2D::GetMomentInertia() const
{
	return m_rotationLocked ? FLT_MAX : m_momentInertia;
}

//////////////////////////////////////////////////////////////////////////
Rigidbody2D::~Rigidbody2D()
{
	GUARANTEE_OR_DIE( m_collider == nullptr, Stringf("Collider not null when Rigidbody2D destroyed") );
}
