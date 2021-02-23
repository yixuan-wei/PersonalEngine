#pragma once

#include "Engine/Math/Vec2.hpp"

struct AABB2;

struct OBB2
{
	Vec2 center;
	Vec2 halfDimensions;
	Vec2 iBasis = Vec2( 1.f, 0.f );

	//construction / deconstruction
	OBB2() = default;
	~OBB2() = default;
	OBB2( const OBB2& copyFrom );
	explicit OBB2( const Vec2& center, const Vec2& fullDimensions, const Vec2& iBasisNormal = Vec2( 1.f, 0.f ) );
	explicit OBB2( const Vec2& center, const Vec2& fullDimensions, float orientationDegrees );
	explicit OBB2( const AABB2& axisAlignedBox, float orientationDegrees = 0.f );

	//Accessors
	bool       IsPointInside( const Vec2& point ) const; //not include edge
	float      GetOrientationDegrees() const;
	float      GetOutterRadius() const; // may just use, halfDim.x+halfDim.y as radius
	float      GetInnerRadius() const;
	const Vec2 GetNearestPoint( const Vec2& refPos ) const;
	const Vec2 GetCenter() const;
	const Vec2 GetDimensions() const;
	const Vec2 GetIBasisNormal() const;
	const Vec2 GetJBasisNormal() const;
	const Vec2 GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const;
	const Vec2 GetUVForPoint( const Vec2& point ) const;
	void       GetCornerPositions( Vec2* out_fourPoints ) const;

	//Mutators
	void Translate( const Vec2& translation );
	void SetCenter( const Vec2& newCenter );
	void SetDimensions( const Vec2& newDimensions );
	void SetOrientationDegrees( float newAbsoluteOrientation );
	void RotateByDegrees( float relativeRotationDegrees );
	void StretchToIncludePoint( const Vec2& point );
	void Fix(); // 1) abs() half dimensions, 2) normalize iBasis

	void operator=( const OBB2& assignFrom );
};