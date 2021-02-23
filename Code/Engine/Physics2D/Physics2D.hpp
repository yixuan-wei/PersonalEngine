#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Physics2D/Collision2D.hpp"
#include "Engine/Core/Delegate.hpp"
#include <vector>

class Rigidbody2D;
class Collider2D;
class DiscCollider2D;
class PolygonCollider2D;
class Clock;
class Timer;

static constexpr int PHYSICS_LAYER_COUNT = 32;

class Physics2D
{
public:
	Delegate<float> onFixedUpdate;

public:
	Physics2D();
	~Physics2D();

	void Startup();
	void BeginFrame();
	void Update();
	void EndFrame();

	void SetParentClock(Clock* clock);
	void SetClockPause(bool isPaused);
	void SetTimeScale(double newTimeScale);
	double GetTimeScale() const;
	bool IsClockPaused() const;

	void EnableLayerInteraction(unsigned int layerIdx0, unsigned int layerIdx1);
	void DisableLayerInteraction(unsigned int layerIdx0, unsigned int layerIdx1);
	bool DoLayersInteract(unsigned int layerIdx0, unsigned int layerIdx1) const;

	void SetSceneGravity(float newGravityAcceleration);
	float GetSceneGravity() const { return m_gravityAcceleration; }

	void SetFixedDeltaTime(double frameTimeSeconds);
	double GetFixedDeltaTime() const { return m_fixedDeltaTime; }

	// factory style create/destroy
	Rigidbody2D* CreateRigidbody();
	void DestroyRigidbody( Rigidbody2D* rb );

	PolygonCollider2D* CreatePolygonCollider( Vec2 const* points, unsigned int pointCount, bool isPointCloud = false );
	DiscCollider2D* CreateDiscCollider( Vec2 const& localPosition, float radius );
	void DestroyCollider( Collider2D* collider );

private:
	void AdvanceSimulation(float deltaSeconds);
	void ApplyEffectors();
	void MoveRigidbodies(float deltaSeconds);
	void DetectCollisions();
	void ResolveCollisions();
	void ResolveCollision(Collision2D& collision);
	void ApplyImpulseInCollision(Collision2D& collision);
	void ApplyImpulseOnce(Collision2D& collision, Vec2 const& impulseBase, float impulseFactor, Vec2 const& contact);
	void CorrectObjectsInCollision(Collision2D& collision);

	float CalculateBiasVelocity(Collision2D const& collision, Vec2 const& contact) const;
	float CalculateTangentImpulse(Collision2D const& collision, Vec2 const& contact) const;
	float CalculateNormalImpulseFactor(Collision2D const& collision, Vec2 const& contact) const;

	void BeginSimulation();
	void InsertCollider(Collider2D* collider);
	void CleanUpDestroyed();

	Clock* m_clock = nullptr;
	Timer* m_stepTimer = nullptr;
	double m_fixedDeltaTime = 1.0/120.0;
	float m_gravityAcceleration = 9.8f;
	std::vector<Collision2D> m_collisions;
	unsigned int m_layerInteractions[PHYSICS_LAYER_COUNT];

public:
	std::vector<Rigidbody2D*> m_rigidbodies;
	std::vector<Collider2D*> m_colliders;
};