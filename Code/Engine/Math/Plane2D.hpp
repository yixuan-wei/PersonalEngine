#pragma once

#include "Engine/Math/Vec2.hpp"

struct Plane2D
{
public:
	Vec2 normal = Vec2(0.f,1.f);
	float distanceFromOriginAlongNormal = 0.f;

public:
	Plane2D(Vec2 const& n, Vec2 const& pointOnPlane);
	Plane2D(Vec2 const& n, float distFromOrigin);

	float GetDistance( Vec2 const& point ) const;
	bool IsPointInFront(Vec2 const& point) const;
};