#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Physics2D/PolygonCollider2D.hpp"
#include "Engine/Physics2D/Manifold2.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

typedef bool (*collision_check_cb)(Collider2D const*, Collider2D const*);
typedef bool (*manifold_get)(Collider2D const*, Collider2D const*, Manifold2* outManifold);

//////////////////////////////////////////////////////////////////////////
static bool DiscVDiscCollisionCheck(Collider2D const* col0, Collider2D const* col1)
{
   // this function is only called if the types tell me these casts are safe - so no need to a dynamic cast or type checks here.
    Disc2 d0 = col0->GetWorldBounds();
    Disc2 d1 = col1->GetWorldBounds();
    float radiusSum = d0.radius + d1.radius;
    if (GetDistanceSquared2D(d0.center, d1.center) < radiusSum * radiusSum) {
        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
static bool DiscVPolygonCollisionCheck(Collider2D const* col0, Collider2D const* col1)
{
    Disc2 discBounds = col0->GetWorldBounds();
    if (col1->Contains(discBounds.center)) {
        return true;
    }

    Vec2 point = col1->GetClosestPoint(discBounds.center);
    return col0->Contains(point);
}

//////////////////////////////////////////////////////////////////////////
static Vec2 GetSupport(Vec2 const& direction, Collider2D const* col0, Collider2D const* col1)
{
    return col0->GetSupport(direction) - col1->GetSupport(-direction);
}

//////////////////////////////////////////////////////////////////////////
static Vec2 TripleCrossDotproduct(Vec2 const& a, Vec2 const& b, Vec2 const& c)
{
    //(a x b) x c
    Vec3 aCrossB = CrossProduct3D(a, b);
    Vec3 result = CrossProduct3D(aCrossB, c);
    return result.GetXY();
}

//////////////////////////////////////////////////////////////////////////
static bool PolygonVPolygonCollisionCheck(Collider2D const* col0, Collider2D const* col1)
{
    //GJK to check collision
    Vec2 center0 = col0->GetWorldBounds().center;
    Vec2 center1 = col1->GetWorldBounds().center;
    Vec2 direction = center1 - center0;

    //init simplex
    Vec2 simplex[3];
    simplex[0] = GetSupport(direction, col0, col1);
    simplex[1] = GetSupport(-direction, col0, col1);
    simplex[2] = GetSupport(direction.GetRotated90Degrees(), col0, col1);

    while (true) {
        Vec2 line0To1 = simplex[1] - simplex[0];
        Vec2 line1To2 = simplex[2] - simplex[1];
        Vec2 line2To0 = simplex[0] - simplex[2];

        Vec2 vertical0To1 = line0To1.GetRotated90Degrees();
        Vec2 vertical1To2 = line1To2.GetRotated90Degrees();
        Vec2 vertical2To0 = line2To0.GetRotated90Degrees();

        Vec2 newVert;
        int replacedIdx = 0;
        if (SignFloat(DotProduct2D(vertical0To1, -simplex[0])) * SignFloat(DotProduct2D(vertical0To1, -line2To0)) < 0.f) {
            //origin outside of line 01
            replacedIdx = 2;
            newVert = GetSupport(SignFloat(DotProduct2D(vertical0To1, line2To0)) * vertical0To1, col0, col1);
        }
        else if (SignFloat(DotProduct2D(vertical1To2, -simplex[1])) * SignFloat(DotProduct2D(vertical1To2, -line0To1)) < 0.f) {
            //origin outside of line 12
            replacedIdx = 0;
            newVert = GetSupport(SignFloat(DotProduct2D(vertical1To2, line0To1)) * vertical1To2, col0, col1);
        }
        else if (SignFloat(DotProduct2D(vertical2To0, -simplex[2])) * SignFloat(DotProduct2D(vertical2To0, -line1To2)) < 0.f) {
            //origin outside of line 20
            replacedIdx = 1;
            newVert = GetSupport(SignFloat(DotProduct2D(vertical2To0, line1To2)) * vertical2To0, col0, col1);
        }
        else {
            //origin inside
            return true;
        }

        //if should end
        if (newVert == simplex[0] || newVert == simplex[1] || newVert == simplex[2]) {
            break;
        }
        else {
            simplex[replacedIdx] = newVert;
        }
   }

    return false;
}

//////////////////////////////////////////////////////////////////////////
static bool DiscVDiscManifoldGet(Collider2D const* col0, Collider2D const* col1, Manifold2* outManifold)
{
    //after mid check, discs must intersect
    Disc2 disc0 = col0->GetWorldBounds();
    Disc2 disc1 = col1->GetWorldBounds();

    float radiusSum = disc0.radius + disc1.radius;
    Vec2 disc0To1 = disc1.center - disc0.center;
    float centerDist = disc0To1.GetLength();

    if (centerDist == 0.f) {
        disc0To1 = Vec2(1.f, 0.f);
    }
    outManifold->penetration = radiusSum - centerDist;
    Vec2 contact = disc0.center + disc0To1.GetClamped(disc0.radius - .5f * outManifold->penetration);
    outManifold->contact.start = contact;
    outManifold->contact.end = contact;
    outManifold->normal = -disc0To1.GetNormalized();

    return true;
}

//////////////////////////////////////////////////////////////////////////
static bool DiscVPolygonManifoldGet(Collider2D const* col0, Collider2D const* col1, Manifold2* outManifold)
{
    PolygonCollider2D const* poly = (PolygonCollider2D const*)col1;

    Disc2 disc = col0->GetWorldBounds();
    bool isDiscInPoly = col1->Contains(disc.center);
    int edgeIdx = -1;
    Vec2 point = poly->GetClosestPoint(disc.center,edgeIdx);
    Vec2 centerToP = point - disc.center;
    float dist = centerToP.GetLength();
    if (dist >= disc.radius && !isDiscInPoly) {
        return false;
    }

    if (isDiscInPoly) {
        outManifold->normal = centerToP.GetNormalized();
        outManifold->penetration = disc.radius + dist;
        centerToP.SetLength(outManifold->penetration * .5f);
        Vec2 contact = disc.center + centerToP;
        outManifold->contact.start = contact;
        outManifold->contact.end = contact;
    }
    else if(dist==0.f){
        outManifold->penetration = disc.radius;
        Vec2 start, end;
        poly->m_polygon.GetEdge(edgeIdx, &start, &end);
        Vec2 normal = (end - start).GetRotatedMinus90Degrees();
        normal.Normalize();
        outManifold->normal = normal;
        Vec2 contact = disc.center + normal * .5f * disc.radius;
        outManifold->contact.start = contact;
        outManifold->contact.end = contact;
    }
    else {
        outManifold->penetration = disc.radius - dist;
        centerToP.Normalize();
        Vec2 contact = disc.center + centerToP * (disc.radius + dist) * .5f;
        outManifold->contact.start = contact;
        outManifold->contact.end = contact;
        outManifold->normal = -centerToP;
    }

    return true;
}

#include "Engine/Renderer/DebugRender.hpp"

//////////////////////////////////////////////////////////////////////////
static bool PolygonVPolygonManifoldGet(Collider2D const* col0, Collider2D const* col1, Manifold2* outManifold)
{
    Vec2 center0 = col0->GetWorldBounds().center;
    Vec2 center1 = col1->GetWorldBounds().center;
    Vec2 direction = center1 - center0;

    //init simplex
    std::vector<Vec2> simplex;
    simplex.push_back(GetSupport(direction, col0, col1));
    simplex.push_back(GetSupport(-direction, col0, col1));
    simplex.push_back(GetSupport(direction.GetRotated90Degrees(), col0, col1));

    //GJK to find start simplex
    bool originInside = false;
    while (true) {
        Vec2 line0To1 = simplex[1] - simplex[0];
        Vec2 line1To2 = simplex[2] - simplex[1];
        Vec2 line2To0 = simplex[0] - simplex[2];

        Vec2 vertical0To1 = line0To1.GetRotated90Degrees();
        Vec2 vertical1To2 = line1To2.GetRotated90Degrees();
        Vec2 vertical2To0 = line2To0.GetRotated90Degrees();

        Vec2 newVert;
        int replacedIdx = 0;
        if (SignFloat(DotProduct2D(vertical0To1, -simplex[0])) * SignFloat(DotProduct2D(vertical0To1, -line2To0)) < 0.f) {
            //origin outside of line 01
            replacedIdx = 2;
            newVert = GetSupport(SignFloat(DotProduct2D(vertical0To1, line2To0)) * vertical0To1, col0, col1);
        }
        else if (SignFloat(DotProduct2D(vertical1To2, -simplex[1])) * SignFloat(DotProduct2D(vertical1To2, -line0To1)) < 0.f) {
            //origin outside of line 12
            replacedIdx = 0;
            newVert = GetSupport(SignFloat(DotProduct2D(vertical1To2, line0To1)) * vertical1To2, col0, col1);
        }
        else if (SignFloat(DotProduct2D(vertical2To0, -simplex[2])) * SignFloat(DotProduct2D(vertical2To0, -line1To2)) < 0.f) {
            //origin outside of line 20
            replacedIdx = 1;
            newVert = GetSupport(SignFloat(DotProduct2D(vertical2To0, line1To2)) * vertical2To0, col0, col1);
        }
        else {
            //origin inside
            originInside = true;
            break;
        }

        //if should end
        if (newVert == simplex[0] || newVert == simplex[1] || newVert == simplex[2]) {
            break;
        }
        else {
            simplex[replacedIdx] = newVert;
        }
    }

    if (!originInside) {
        return false;
    }

    //find closest edge
    float closestDist = FLT_MAX;
    Vec2 closestNormal;
    size_t closestIndex = 0;
    while (true) {
        for (size_t i = 0; i < simplex.size(); i++) {
            size_t j = i + 1;
            if (j == simplex.size()) {
                j = 0;
            }

            Vec2 start = -simplex[i];
            Vec2 edge = simplex[j] + start;
            Vec2 normal = TripleCrossDotproduct(edge, start, edge).GetNormalized();
            float dist = DotProduct2D(normal, start);
            if (dist < closestDist) {
                closestDist = dist;
                closestNormal = normal;
                closestIndex = i;
            }
        }

        Vec2 newVert = GetSupport(-closestNormal, col0, col1);
        for (size_t i = 0; i < simplex.size(); i++) {
            if (newVert == simplex[i]) {
                //already find the closest
                outManifold->normal = closestNormal;
                outManifold->penetration = closestDist;

                PolygonCollider2D const* poly0 = static_cast<PolygonCollider2D const*>(col0);
                LineSegment2 edge0 = poly0->GetContactEdge(-closestNormal,closestDist);

                PolygonCollider2D const* poly1 = static_cast<PolygonCollider2D const*>(col1);
                LineSegment2 edge1 = poly1->GetContactEdge(closestNormal,closestDist);

                outManifold->contact = ClipLineSegmentToLineSegment(edge1, edge0);

                return true;
            }
        }

        //need expand
        simplex.insert(simplex.begin() + closestIndex + 1, newVert);
    }
}

//////////////////////////////////////////////////////////////////////////
// a "matrix" lookup is just a 2D array
static collision_check_cb CollisionChecks[NUM_COLLIDER2D_TYPES*NUM_COLLIDER2D_TYPES] = {
    /*             disc,                        polygon, */
    /*    disc */  DiscVDiscCollisionCheck,     nullptr,
    /* polygon */  DiscVPolygonCollisionCheck,  PolygonVPolygonCollisionCheck
};

//////////////////////////////////////////////////////////////////////////
static manifold_get ManifoldGets[NUM_COLLIDER2D_TYPES * NUM_COLLIDER2D_TYPES] = {
    /*             disc,                        polygon, */
    /*    disc */  DiscVDiscManifoldGet,        nullptr,
    /* polygon */  DiscVPolygonManifoldGet,     PolygonVPolygonManifoldGet
};

//////////////////////////////////////////////////////////////////////////
static bool MidPhaseCollisionCheck(Collider2D const* first, Collider2D const* second)
{
    Disc2 firstBounds = first->GetWorldBounds();
    Disc2 secondBounds = second->GetWorldBounds();
    
    if (DoDiscsOverlap2D(firstBounds.center, firstBounds.radius, secondBounds.center, secondBounds.radius)) {
        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
bool Collider2D::Intersects(Collider2D const* other) const
{
    //Mid phase manifold
    if (!MidPhaseCollisionCheck(this, other)) {
        return false;
    }

    eCollider2DType myType = m_type;
    eCollider2DType otherType = other->GetType();

    if (myType <= otherType) {
        int idx = otherType * NUM_COLLIDER2D_TYPES + myType;
        collision_check_cb check = CollisionChecks[idx];
        return check(this, other);
    }
    else {
    // flip the types when looking into the index.
        int idx = myType * NUM_COLLIDER2D_TYPES + otherType;
        collision_check_cb check = CollisionChecks[idx];
        return check(other, this);
    }
}

//////////////////////////////////////////////////////////////////////////
bool Collider2D::GetManifold(Collider2D const* other, Manifold2* outManifold) const
{
    //mid phase manifold
    if (!MidPhaseCollisionCheck(this, other)) {
        return false;
    }

    eCollider2DType myType = m_type;
    eCollider2DType otherType = other->GetType();

    if (myType <= otherType) {
        int idx = otherType * NUM_COLLIDER2D_TYPES + myType;
        manifold_get manifold = ManifoldGets[idx];
        return manifold(this, other,outManifold);
    }
    else {
    // flip the types when looking into the index. and the normal
        int idx = myType * NUM_COLLIDER2D_TYPES + otherType;
        manifold_get manifold = ManifoldGets[idx];
        bool result = manifold(other, this,outManifold);
        outManifold->normal = -outManifold->normal;
        return result;
    }
}

//////////////////////////////////////////////////////////////////////////
float Collider2D::GetBounceWith(Collider2D const* other) const
{
    switch (m_physicsMaterial.bounceCombine) {
    case ePhysicsCombineType::AVERAGE:
        return (m_physicsMaterial.bounciness + other->m_physicsMaterial.bounciness) * .5f;
    case ePhysicsCombineType::MULTIPLY:
        return m_physicsMaterial.bounciness * other->m_physicsMaterial.bounciness;
    }
    return m_physicsMaterial.bounciness;
}

//////////////////////////////////////////////////////////////////////////
float Collider2D::GetFrictionWith(Collider2D const* other) const
{
    switch (m_physicsMaterial.frictionCombine) {
    case ePhysicsCombineType::AVERAGE:
        return (m_physicsMaterial.friction + other->m_physicsMaterial.friction) * .5f;
    case ePhysicsCombineType::MULTIPLY:
        return m_physicsMaterial.friction * other->m_physicsMaterial.friction;
    }
    return m_physicsMaterial.friction;
}

//////////////////////////////////////////////////////////////////////////
float Collider2D::GetMass() const
{
    return m_rigidbody->GetMass();
}

//////////////////////////////////////////////////////////////////////////
eSimulationMode Collider2D::GetSimulationMode() const
{
    return m_rigidbody->GetSimulationMode();
}

//////////////////////////////////////////////////////////////////////////
void Collider2D::SetIsTrigger(bool isTrigger)
{
    m_isTrigger = isTrigger;
}

//////////////////////////////////////////////////////////////////////////
Collider2D::~Collider2D() {}
