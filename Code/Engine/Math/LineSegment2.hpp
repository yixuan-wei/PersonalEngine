#pragma once

#include "Engine/Math/Vec2.hpp"

struct LineSegment2
{
public:
	Vec2 start;
	Vec2 end;

public:
	LineSegment2() = default;
	~LineSegment2() = default;
	explicit LineSegment2( const Vec2& start, const Vec2& end );

	const Vec2 GetCenter() const;
	const Vec2 GetNearestPoint(const Vec2& point) const;
	float GetLength() const;
	float GetLengthSquared() const;
	bool IsSegmentPoint() const;

	void Translate(const Vec2& translation);
	void RotateDegreesAboutCenter(float deltaDegrees);
};