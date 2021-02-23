#pragma once

#include "Engine/Math/Vec2.hpp"

struct Capsule2
{
public:
	Vec2  boneStart;
	Vec2  boneEnd;
	float radius = 0.f;

public:
	Capsule2() = default;
	~Capsule2() = default;
	explicit Capsule2( const Vec2& startPos, const Vec2& endPos, float capsuleRadius );

	//Accessors
	bool IsPointInside( const Vec2& point )const;
	const Vec2 GetNearestPoint(const Vec2& point) const;
	const Vec2 GetCenter() const;

	//Mutators
	void Translate(const Vec2& translation);
	void RotateDegreesAboutCenter(float deltaDegrees);
};