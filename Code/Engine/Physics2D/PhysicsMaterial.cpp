#include "Engine/Physics2D/PhysicsMaterial.hpp"

//////////////////////////////////////////////////////////////////////////
PhysicsMaterial::PhysicsMaterial(float restitution, float inputFriction, ePhysicsCombineType restitutionCombine, ePhysicsCombineType fCombine)
    :bounciness(restitution)
    ,friction(inputFriction)
    ,bounceCombine(restitutionCombine)
    ,frictionCombine(fCombine)
{
}
