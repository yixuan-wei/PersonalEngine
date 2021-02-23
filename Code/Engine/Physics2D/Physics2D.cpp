#include "Engine/Physics2D/Physics2D.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Physics2D/PolygonCollider2D.hpp"
#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Timer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/Vec3.hpp"

static std::vector<Collision2D> sCollisions;
static std::vector<bool> sStillCollides;

//////////////////////////////////////////////////////////////////////////
//Static methods
//////////////////////////////////////////////////////////////////////////
static void AppendAndFireEventsForNewCollision(Collision2D const& collision)
{
    for (size_t i = 0; i < sCollisions.size(); i++) {
        Collision2D& col = sCollisions[i];
        if (col.me == collision.me && col.other == collision.other) {
            Collision2D invCol = col.GetInverse();            
            if (col.me->IsTrigger()) {
                col.me->OnTriggerStay(col.me, col.other);
            } 
            if (col.other->IsTrigger()) {
                col.other->OnTriggerStay(col.other, col.me);
            }
            if (!col.me->IsTrigger() && !col.other->IsTrigger()) {
                col.me->m_rigidbody->onOverlapStay(col);
                col.other->m_rigidbody->onOverlapStay(invCol);
            }

            sCollisions[i] = collision;
            sStillCollides[i] = true;
            return;
        }
    }

    Collision2D invCol = collision.GetInverse();
    if (!collision.me->IsTrigger() && !collision.other->IsTrigger()) {
        collision.me->m_rigidbody->onOverlapStart(collision);
        collision.other->m_rigidbody->onOverlapStart(invCol);
    }
    if (collision.me->IsTrigger()) {
        collision.me->OnTriggerEnter(collision.me, collision.other);
    }
    if (collision.other->IsTrigger()) {
        collision.other->OnTriggerEnter(collision.other, collision.me);
    }
    sCollisions.push_back(collision);
    sStillCollides.push_back(true);
}

//////////////////////////////////////////////////////////////////////////
static void EraseAndFireEventsForOldCollision(Collision2D const& collision)
{
    for (size_t i = 0; i < sCollisions.size(); i++) {
        Collision2D& col = sCollisions[i];
        if (col.me == collision.me && col.other == collision.other) {
            Collision2D invCol = col.GetInverse();
            if (!col.me->IsTrigger() && !col.other->IsTrigger()) {
                col.me->m_rigidbody->onOverlapStop(col);
                col.other->m_rigidbody->onOverlapStop(invCol);
            }
            if (col.me->IsTrigger()) {
                col.me->OnTriggerLeave(col.me, col.other);
            }
            if (col.other->IsTrigger()) {
                col.other->OnTriggerLeave(col.other, col.me);
            }

            sCollisions.erase(sCollisions.begin() + i);
            sStillCollides.erase(sStillCollides.begin() + i);
            return;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
static void CleanUpPastCollisions()
{
    for (size_t i = 0; i < sCollisions.size();) {
        if (!sStillCollides[i]) {
            Collision2D& col = sCollisions[i];
            if (!col.me->IsTrigger() && !col.other->IsTrigger()) {
                col.me->m_rigidbody->onOverlapStop(col);
                col.other->m_rigidbody->onOverlapStop(col.GetInverse());
            }
            if (col.me->IsTrigger()) {
                col.me->OnTriggerLeave(col.me, col.other);
            }
            if (col.other->IsTrigger()) {
                col.other->OnTriggerLeave(col.other, col.me);
            }

            sCollisions.erase(sCollisions.begin() + i);
            sStillCollides.erase(sStillCollides.begin() + i);
        }
        else {
            sStillCollides[i] = false;
            i++;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//Class Methods
//////////////////////////////////////////////////////////////////////////
Physics2D::Physics2D()
{
    m_clock = new Clock();
    m_stepTimer = new Timer();
}

//////////////////////////////////////////////////////////////////////////
Physics2D::~Physics2D()
{
    for (size_t idx = 0; idx < m_rigidbodies.size(); idx++) {
        if (m_rigidbodies[idx] != nullptr) {
            m_rigidbodies[idx]->Destroy();
        }
    }

    CleanUpDestroyed();
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::Startup()
{
    m_stepTimer->SetTimerSeconds(m_clock, m_fixedDeltaTime);
    for (int i = 0; i < PHYSICS_LAYER_COUNT; i++) {
        m_layerInteractions[i] = 0xffffffff;
    }
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::BeginFrame()
{
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::Update()
{
    while (m_stepTimer->CheckAndDecrement()) {
        AdvanceSimulation((float)m_fixedDeltaTime);
    }
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::EndFrame()
{
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::SetParentClock(Clock* clock)
{
    m_clock->SetParent(clock);
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::SetClockPause(bool isPaused)
{
    if (isPaused) {
        m_clock->Pause();
    }
    else {
        m_clock->Resume();
    }
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::SetTimeScale(double newTimeScale)
{
    m_clock->SetScale(newTimeScale);
}

//////////////////////////////////////////////////////////////////////////
double Physics2D::GetTimeScale() const
{
    return m_clock->GetScale();
}

//////////////////////////////////////////////////////////////////////////
bool Physics2D::IsClockPaused() const
{
    return m_clock->IsPaused();
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::EnableLayerInteraction(unsigned int layerIdx0, unsigned int layerIdx1)
{
    m_layerInteractions[layerIdx0] = (1 << layerIdx1) & m_layerInteractions[layerIdx0];
    m_layerInteractions[layerIdx1] = (1 << layerIdx0) & m_layerInteractions[layerIdx1];
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::DisableLayerInteraction(unsigned int layerIdx0, unsigned int layerIdx1)
{
    m_layerInteractions[layerIdx0] = (~(1 << layerIdx1)) & m_layerInteractions[layerIdx0];
    m_layerInteractions[layerIdx1] = (~(1 << layerIdx0)) & m_layerInteractions[layerIdx1];
}

//////////////////////////////////////////////////////////////////////////
bool Physics2D::DoLayersInteract(unsigned int layerIdx0, unsigned int layerIdx1) const
{
    return (m_layerInteractions[layerIdx0] & (1 << layerIdx1)) != 0;
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::SetSceneGravity(float newGravityAcceleration)
{
	m_gravityAcceleration = newGravityAcceleration;
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::SetFixedDeltaTime(double frameTimeSeconds)
{
    m_fixedDeltaTime = frameTimeSeconds;
    m_stepTimer->SetTimerSeconds(m_fixedDeltaTime);
}

//////////////////////////////////////////////////////////////////////////
Rigidbody2D* Physics2D::CreateRigidbody()
{
	Rigidbody2D* rigidbody = new Rigidbody2D();
	rigidbody->m_system = this;
	for( size_t rIdx = 0; rIdx < m_rigidbodies.size(); rIdx++ )
	{
		if( m_rigidbodies[rIdx] == nullptr )
		{
			m_rigidbodies[rIdx] = rigidbody;
			return rigidbody;
		}
	}

	m_rigidbodies.push_back( rigidbody );
	return rigidbody;
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::DestroyRigidbody( Rigidbody2D* rb )
{
	rb->m_isDestroyed = true;

	Collider2D* collider = rb->GetCollider();
	if( collider != nullptr )
	{
		rb->TakeCollider( nullptr );
	}
}

//////////////////////////////////////////////////////////////////////////
PolygonCollider2D* Physics2D::CreatePolygonCollider(Vec2 const* points, unsigned int pointCount, bool isPointCloud)
{
    PolygonCollider2D* pCollider = nullptr;
    if (isPointCloud)
    {
        Polygon2D poly = Polygon2D::MakeFromPointCloud(points, pointCount);
        pCollider = new PolygonCollider2D(poly);
    }
    else
    {
        pCollider = new PolygonCollider2D(points, pointCount);
    }
	pCollider->m_system = this;
	InsertCollider((Collider2D*)pCollider);
	return pCollider;
}

//////////////////////////////////////////////////////////////////////////
DiscCollider2D* Physics2D::CreateDiscCollider( Vec2 const& localPosition, float radius )
{
	DiscCollider2D* dCollider = new DiscCollider2D(localPosition,radius);
	dCollider->m_system = this;
	InsertCollider((Collider2D*)dCollider);
	return dCollider;
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::DestroyCollider( Collider2D* collider )
{ 
	collider->m_isDestroyed = true;
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::AdvanceSimulation(float deltaSeconds)
{
    onFixedUpdate(deltaSeconds);
    
    DetectCollisions();
    for (Collision2D col : m_collisions) {
        AppendAndFireEventsForNewCollision(col);
    }
    for (size_t i = 0; i < m_collisions.size();i++) {
        Collision2D& col = m_collisions[i];
        Rigidbody2D* me = col.me->m_rigidbody;
        Rigidbody2D* other = col.other->m_rigidbody;
        if (me->m_isDestroyed || !me->m_isEnabled ||
            other->m_isDestroyed || !other->m_isEnabled) {
            EraseAndFireEventsForOldCollision(col);
        }
    }

    ResolveCollisions();
    for (Collision2D col : m_collisions) {
        if (!col.me->IsTrigger() && !col.other->IsTrigger()) {
            Collision2D invCol = col.GetInverse();
            col.me->m_rigidbody->onCollision(col);
            col.other->m_rigidbody->onCollision(invCol);
        }
    }

    BeginSimulation();
    ApplyEffectors();//global forces to dynamic rigid bodies
    MoveRigidbodies(deltaSeconds);//apply euler step to all rigid bodies

    CleanUpPastCollisions();
    CleanUpDestroyed();//clean up destroyed objects
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::ApplyEffectors()
{
    //gravity
    Vec2 gravityAcceleration = m_gravityAcceleration * Vec2(0.f, -1.f);
    for (size_t idx = 0; idx < m_rigidbodies.size(); idx++)
    {
        Rigidbody2D* rigidbody = m_rigidbodies[idx];
        if (rigidbody == nullptr || rigidbody->m_mode != eSimulationMode::DYNAMIC || 
            rigidbody->m_isDestroyed || !rigidbody->m_isEnabled)
        {
            continue;
        }

        rigidbody->ApplyGravityForce(gravityAcceleration);
        rigidbody->ApplyDragForce();
    }
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::MoveRigidbodies(float deltaSeconds)
{
    for (size_t idx = 0; idx < m_rigidbodies.size(); idx++)
    {
        Rigidbody2D* rigidbody = m_rigidbodies[idx];
        if (rigidbody != nullptr && rigidbody->m_mode!= eSimulationMode::STATIC && 
            !rigidbody->m_isDestroyed && rigidbody->m_isEnabled)
        {
            rigidbody->Simulate(deltaSeconds);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::DetectCollisions()
{
    m_collisions.clear();

    for (size_t i = 0; i < m_colliders.size(); i++) {
        for (size_t j = i + 1; j < m_colliders.size(); j++) {
            Collider2D* first = m_colliders[i];
            Collider2D* second = m_colliders[j];
            if (first == nullptr || second == nullptr) {
                continue;
            }

            Rigidbody2D* rigid1 = first->m_rigidbody;
            Rigidbody2D* rigid2 = second->m_rigidbody;
            if ( rigid1->IsDestroyed() || rigid2->IsDestroyed() ||
                !rigid1->IsEnabled() || !rigid2->IsEnabled() ||
                !DoLayersInteract(rigid1->GetLayer(), rigid2->GetLayer())) {
                continue;
            }

            Manifold2 temp;
            if (first->GetManifold(second, &temp)) {
                Collision2D collision = Collision2D(first, second, temp);
                m_collisions.push_back(collision);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::ResolveCollisions()
{
    for (size_t idx = 0; idx < m_collisions.size(); idx++) {
        ResolveCollision(m_collisions[idx]);
    }
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::ResolveCollision(Collision2D& collision)
{
    if (collision.me->IsTrigger() || collision.other->IsTrigger()) {
        return;
    }

    //sequential impulse, accumulate impulse at collision
    for (int i = 0; i < APPLY_IMPULSE_ITERATIONS; i++) {
        ApplyImpulseInCollision(collision);
    }

    //apply bounciness
    float bounceE = collision.me->GetBounceWith(collision.other);
    ApplyImpulseOnce(collision, collision.manifold.normal, bounceE*collision.manifold.bounceImpulse.x, collision.manifold.contact.start);
    if (!collision.manifold.contact.IsSegmentPoint()) {
        ApplyImpulseOnce(collision, collision.manifold.normal, bounceE * collision.manifold.bounceImpulse.y, collision.manifold.contact.end);
    }
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::ApplyImpulseInCollision(Collision2D& collision)
{
    //start contact
    Vec2 contactStart = collision.manifold.contact.start;
    Vec2 velDiff= collision.other->m_rigidbody->GetImpactVelocity(contactStart) - collision.me->m_rigidbody->GetImpactVelocity(contactStart);
    //normal
    Vec2 normal = collision.manifold.normal;
    float biasV = CalculateBiasVelocity(collision, contactStart);
    float normalFactor = CalculateNormalImpulseFactor(collision, contactStart);
    float normalImpulse0 = DotProduct2D(velDiff,normal) * normalFactor;
    float prevNormal = collision.manifold.bounceImpulse.x;
    collision.manifold.bounceImpulse.x = MaxFloat(normalImpulse0 + prevNormal, 0.f);
    normalImpulse0 = collision.manifold.bounceImpulse.x - prevNormal;
    normalImpulse0 += biasV*normalFactor;
    collision.manifold.normalImpulse.x += normalImpulse0;
    ApplyImpulseOnce(collision, normal, normalImpulse0, contactStart);
    //friction
    float tangentImpulse = CalculateTangentImpulse(collision, contactStart);
    float frictionE = collision.me->GetFrictionWith(collision.other);
    float maxTangentImpulse = collision.manifold.normalImpulse.x * frictionE;
    float prevTangent = collision.manifold.tangentImpulse.x;
    collision.manifold.tangentImpulse.x = Clamp(prevTangent + tangentImpulse, -maxTangentImpulse, maxTangentImpulse);
    tangentImpulse = collision.manifold.tangentImpulse.x - prevTangent;
    Vec2 friction = normal.GetRotated90Degrees();
    ApplyImpulseOnce(collision, friction, tangentImpulse, contactStart);

    //end contact
    Vec2 contactEnd = collision.manifold.contact.end;
    if (contactStart != contactEnd) {
        //normal
        biasV = CalculateBiasVelocity(collision, contactEnd);
        velDiff = collision.other->m_rigidbody->GetImpactVelocity(contactEnd) - collision.me->m_rigidbody->GetImpactVelocity(contactEnd);
        normalFactor = CalculateNormalImpulseFactor(collision, contactEnd);
        float normalImpulse1 = DotProduct2D(velDiff,normal)*normalFactor;
        prevNormal = collision.manifold.bounceImpulse.y;
        collision.manifold.bounceImpulse.y = MaxFloat(normalImpulse1 + prevNormal,0.f);
        normalImpulse1 = collision.manifold.bounceImpulse.y - prevNormal;
        normalImpulse1 += biasV*normalFactor;
        collision.manifold.normalImpulse.y += normalImpulse1;
        ApplyImpulseOnce(collision, normal, normalImpulse1, contactEnd);
        //tangent
        tangentImpulse = CalculateTangentImpulse(collision, contactEnd);
        maxTangentImpulse = collision.manifold.normalImpulse.y * frictionE;
        prevTangent = collision.manifold.tangentImpulse.y;
        collision.manifold.tangentImpulse.y = Clamp(prevTangent + tangentImpulse, -maxTangentImpulse, maxTangentImpulse);
        tangentImpulse = collision.manifold.tangentImpulse.y - prevTangent;
        ApplyImpulseOnce(collision, friction, tangentImpulse, contactEnd);
    }
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::ApplyImpulseOnce(Collision2D& collision, Vec2 const& impulseBase, float impulseFactor, Vec2 const& contact)
{
    Collider2D* first = collision.me;
    Collider2D* second = collision.other;
    if (first->GetSimulationMode() == eSimulationMode::DYNAMIC && second->GetSimulationMode() == eSimulationMode::DYNAMIC) {
        first->m_rigidbody->ApplyImpulseAt(impulseFactor * impulseBase, contact);
        second->m_rigidbody->ApplyImpulseAt(-impulseFactor * impulseBase, contact);
    }
    else if (first->GetSimulationMode() == eSimulationMode::DYNAMIC) {
        first->m_rigidbody->ApplyImpulseAt(impulseFactor * impulseBase, contact);
    }
    else if (second->GetSimulationMode() == eSimulationMode::DYNAMIC) {
        second->m_rigidbody->ApplyImpulseAt(-impulseFactor * impulseBase, contact);
    }
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::CorrectObjectsInCollision(Collision2D& collision)
{
    float mass1 = collision.me->GetMass();
    float mass2 = collision.other->GetMass();

    float pene1 = 0.f;
    float pene2 = 0.f;
    float massSum = mass1 + mass2;
    if (massSum <= 0.f) {
        massSum = 1.f;
        pene1 = 1.f;
    }
    else {
        pene1 = mass2  / massSum;
        pene2 = 1.f - pene1;
    }

    eSimulationMode mode1 = collision.me->GetSimulationMode();
    eSimulationMode mode2 = collision.other->GetSimulationMode();
    float totalPene = collision.manifold.penetration;
    Vec2 normal1 = collision.manifold.normal;
    Vec2 normal2 = -normal1;
    //dynamic vs dynamic
    if ((mode1 == eSimulationMode::DYNAMIC && mode2 == eSimulationMode::DYNAMIC) ||
        (mode1==eSimulationMode::KINEMATIC && mode2==eSimulationMode::KINEMATIC)) {
        collision.me->m_rigidbody->Move(pene1 * totalPene * normal1);
        collision.other->m_rigidbody->Move(pene2 * totalPene * normal2);
    }
    //pushes only me
    else if (mode1 == eSimulationMode::DYNAMIC ||
        (mode1 == eSimulationMode::KINEMATIC && mode2 == eSimulationMode::STATIC)) {
        collision.me->m_rigidbody->Move(totalPene * normal1);
    }
    //pushes only other
    else if (mode2 == DYNAMIC ||
        (mode1 == eSimulationMode::STATIC && mode2 == eSimulationMode::KINEMATIC)) {
        collision.other->m_rigidbody->Move(totalPene * normal2);
    }
}

//////////////////////////////////////////////////////////////////////////
float Physics2D::CalculateBiasVelocity(Collision2D const& collision, Vec2 const& contact) const
{
    float vBias = 0.f;
    float radius = MinFloat(collision.me->GetWorldBounds().radius, collision.other->GetWorldBounds().radius);
    Vec2 contactDir = collision.manifold.contact.end - collision.manifold.contact.start;
    float deltaContact = DotProduct2D(contactDir, collision.manifold.normal);
    if (collision.manifold.contact.IsSegmentPoint() ||
        (deltaContact >= 0.f && contact == collision.manifold.contact.start) ||
        (deltaContact <= 0.f && contact == collision.manifold.contact.end))
    {
        vBias = .2f / (float)m_fixedDeltaTime * MaxFloat(0.f, collision.manifold.penetration-0.01f*radius);
    }
    else {
        vBias = .2f / (float)m_fixedDeltaTime * MaxFloat(0.f, collision.manifold.penetration - AbsFloat(deltaContact)-0.01f*radius);
    }
    vBias = MinFloat(0.05f,vBias);
    vBias = vBias < 0.01f ? 0.f : vBias;
    return vBias;
}

//////////////////////////////////////////////////////////////////////////
float Physics2D::CalculateTangentImpulse(Collision2D const& collision, Vec2 const& contact) const
{
    Collider2D* me = collision.me;
    Collider2D* other = collision.other;

    Vec2 velDiff = other->m_rigidbody->GetImpactVelocity(contact) - me->m_rigidbody->GetImpactVelocity(contact);

    float mass1 = me->GetMass();
    float mass2 = other->GetMass();
    float massSum = mass1 + mass2;
    float massFactor = 0.f;

    Vec2 normal = collision.manifold.normal;
    Vec2 tangent = normal.GetRotated90Degrees();
    float inertia1 = me->m_rigidbody->GetMomentInertia();
    float inertia2 = other->m_rigidbody->GetMomentInertia();

    Vec2 center1toP = contact - me->m_rigidbody->GetWorldPosition();
    Vec2 center1toPTan = center1toP.GetRotated90Degrees();
    float angular1DotTangent = DotProduct2D(center1toPTan, tangent);
    float angular1Tangent = angular1DotTangent * angular1DotTangent / inertia1;

    Vec2 center2toP = contact - other->m_rigidbody->GetWorldPosition();
    Vec2 center2ToPTan = center2toP.GetRotated90Degrees();
    float angular2DotTangent = DotProduct2D(center2ToPTan, tangent);
    float angular2Tangent = angular2DotTangent * angular2DotTangent / inertia2;

    if (me->GetSimulationMode() != eSimulationMode::DYNAMIC && other->GetSimulationMode() == eSimulationMode::DYNAMIC) {
        massFactor = 1.f / mass2;
        angular1Tangent = 0.f;
    }
    else if (me->GetSimulationMode() == eSimulationMode::DYNAMIC && other->GetSimulationMode() != eSimulationMode::DYNAMIC) {
        massFactor = 1.f / mass1;
        angular2Tangent = 0.f;
    }
    else {
        massFactor = massSum / (mass1 * mass2);
    }

    //float bounceE = me->GetBounceWith(other) + 1.f;

    return DotProduct2D(velDiff, tangent) / (massFactor + angular1Tangent + angular2Tangent);
}

//////////////////////////////////////////////////////////////////////////
float Physics2D::CalculateNormalImpulseFactor(Collision2D const& collision, Vec2 const& contact) const
{
    Collider2D* me = collision.me;
    Collider2D* other = collision.other;

    float mass1 = me->GetMass();
    float mass2 = other->GetMass();
    float massSum = mass1 + mass2;
    float massFactor = 0.f;

    Vec2 normal = collision.manifold.normal;
    float inertia1 = me->m_rigidbody->GetMomentInertia();
    float inertia2 = other->m_rigidbody->GetMomentInertia();

    Vec2 center1toP = contact - me->m_rigidbody->GetWorldPosition();
    Vec2 center1toPTan = center1toP.GetRotated90Degrees();
    float angular1DotNormal = DotProduct2D(center1toPTan, normal);
    float angular1Normal = angular1DotNormal * angular1DotNormal / inertia1;

    Vec2 center2toP = contact - other->m_rigidbody->GetWorldPosition();
    Vec2 center2ToPTan = center2toP.GetRotated90Degrees();
    float angular2DotNormal = DotProduct2D(center2ToPTan, normal);
    float angular2Normal = angular2DotNormal * angular2DotNormal / inertia2;

    if (me->GetSimulationMode() != eSimulationMode::DYNAMIC && other->GetSimulationMode() == eSimulationMode::DYNAMIC) {
        massFactor = 1.f / mass2;
        angular1Normal = 0.f;
    }
    else if (me->GetSimulationMode() == eSimulationMode::DYNAMIC && other->GetSimulationMode() != eSimulationMode::DYNAMIC) {
        massFactor = 1.f / mass1;
        angular2Normal = 0.f;
    }
    else {
        massFactor = massSum / (mass1 * mass2);
    }

    return 1.f / (massFactor + angular1Normal + angular2Normal);
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::BeginSimulation()
{
    for (size_t idx = 0; idx < m_rigidbodies.size(); idx++) {
        Rigidbody2D* rigidbody = m_rigidbodies[idx];
        if (rigidbody != nullptr && !rigidbody->IsDestroyed()) {
            rigidbody->BeginSimulation();
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::InsertCollider(Collider2D* collider)
{
    for (size_t cIdx = 0; cIdx < m_colliders.size(); cIdx++)
    {
        if (m_colliders[cIdx] == nullptr)
        {
            m_colliders[cIdx] = collider;
			return;
        }
    }

    m_colliders.push_back(collider);
}

//////////////////////////////////////////////////////////////////////////
void Physics2D::CleanUpDestroyed()
{
    //clean out destroyed colliders
    for (size_t cIdx = 0; cIdx < m_colliders.size(); cIdx++)
    {
        if (m_colliders[cIdx]!=nullptr && m_colliders[cIdx]->m_isDestroyed)
        {
            delete m_colliders[cIdx];
            m_colliders[cIdx] = nullptr;
        }
    }
    //clean up destroyed objects
    for (size_t rIdx = 0; rIdx < m_rigidbodies.size(); rIdx++)
    {
        if (m_rigidbodies[rIdx]!=nullptr && m_rigidbodies[rIdx]->m_isDestroyed)
        {
            delete m_rigidbodies[rIdx];
            m_rigidbodies[rIdx] = nullptr;
        }
    }
}

