#pragma once

#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/EngineCommon.hpp"

struct AABB2
{
public:
	Vec2 mins;
	Vec2 maxs;

	static const AABB2 ZERO_TO_ONE;

public:
	AABB2() = default;
	~AABB2() = default;
	AABB2( const AABB2& copyFrom );
	explicit AABB2( const Vec2& inMins, const Vec2& inMaxs );
	explicit AABB2( float minX, float minY, float maxX, float maxY );

	void SetFromText( const char* text );

	bool  IsBoundsOverlap(AABB2 const& bounds) const;
	bool  IsBoundsInside(AABB2 const& bounds) const;
	bool  IsPointInside( const Vec2& point ) const;
	Vec2  GetCenter() const;
	Vec2  GetDimensions() const;
	Vec2  GetNearestPoint( const Vec2 referencePos ) const;
	Vec2  GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const;
	Vec2  GetUVForPoint( const Vec2& point ) const;
	AABB2 GetBoxWithin( const Vec2& dimensions, const Vec2& alignment )const;
	AABB2 GetBoxAtRight( float fractionOfWidth, float additionalWidth = 0.f )const;
	AABB2 GetBoxAtLeft( float fractionOfWidth, float additionalWidth = 0.f )const;
	AABB2 GetBoxAtTop( float fractionOfHeight, float additionalHeight = 0.f )const;
	AABB2 GetBoxAtBottom( float fractionOfHeight, float additionalHeight = 0.f )const;
	float GetOutterRadius() const; // may just use, halfDim.x+halfDim.y as radius
	float GetInnerRadius() const;
	void  GetCornerPositions( Vec2* out_fourPoints ) const;

	void  Translate( const Vec2& translation );
	void  SetCenter( const Vec2& newCenter );
	void  SetDimensions( const Vec2& newDimension );
	void  StretchToIncludePoint( const Vec2& point );
	void  StretchToIncludeBounds(AABB2 const& bounds);
	void  FitWithinBounds( const AABB2& bounds );
	void  FitInBoundsAndResize( const AABB2& bounds );
	void  AlignWithinBox( const AABB2& box, const Vec2& alignment );
	void  FitTightlyWithinBox( const AABB2& box, const Vec2& alignment = ALIGN_CENTERED );
	AABB2 ChopBoxOffRight( float fractionOfWidth, float additionalWidth = 0.f );
	AABB2 ChopBoxOffLeft( float fractionOfWidth, float additionalWidth = 0.f );
	AABB2 ChopBoxOffTop( float fractionOfHeight, float additionalHeight = 0.f );
	AABB2 ChopBoxOffBottom( float fractionOfHeight, float additionalHeight = 0.f );

	bool operator==( const AABB2& compareWith ) const;
	bool operator!=( const AABB2& compareWith ) const;
	void operator=( const AABB2& assignFrom );

};