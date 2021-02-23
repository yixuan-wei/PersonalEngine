#pragma once

enum ePhysicsCombineType {
    AVERAGE,
    MULTIPLY
};

struct PhysicsMaterial 
{
    float bounciness = 1.f;
    float friction = .0f;
    ePhysicsCombineType bounceCombine = ePhysicsCombineType::MULTIPLY;
    ePhysicsCombineType frictionCombine = ePhysicsCombineType::MULTIPLY;

    PhysicsMaterial() = default;
    PhysicsMaterial(float restitution, float inputFriction = 0.f, ePhysicsCombineType restitutionCombine = ePhysicsCombineType::MULTIPLY, ePhysicsCombineType fCombine = ePhysicsCombineType::MULTIPLY);
};