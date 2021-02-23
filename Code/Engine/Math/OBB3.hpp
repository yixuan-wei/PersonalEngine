#pragma 

#include "Engine/Math/Vec3.hpp"

struct AABB3;

struct OBB3 
{
public:
    Vec3 center;
    Vec3 halfDimensions;
    Vec3 right = Vec3(1.f,0.f,0.f);
    Vec3 up = Vec3(0.f,1.f,0.f);

public:
    OBB3(Vec3 const& c, Vec3 const& hDim, Vec3 const& rightDir = Vec3(1.f,0.f,0.f), Vec3 const& upDir = Vec3(0.f,1.f,0.f));
    explicit OBB3(AABB3 const& aabb);

    Vec3 GetForwardVector() const;
};