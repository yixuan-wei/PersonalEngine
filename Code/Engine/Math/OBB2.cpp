#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"

//////////////////////////////////////////////////////////////////////////
OBB2::OBB2( const OBB2& copyFrom )
	:center(copyFrom.center)
	,halfDimensions(copyFrom.halfDimensions)
	,iBasis(copyFrom.iBasis)
{
}

//////////////////////////////////////////////////////////////////////////
OBB2::OBB2( const Vec2& center, const Vec2& fullDimensions, const Vec2& iBasisNormal /*= Vec2( 1.f, 0.f ) */ )
	:center( center)
	,halfDimensions(fullDimensions*.5f)
	,iBasis(iBasisNormal)
{
}

//////////////////////////////////////////////////////////////////////////
OBB2::OBB2( const Vec2& center, const Vec2& fullDimensions, float orientationDegrees )
	:center(center)
	,halfDimensions(fullDimensions*.5f)
	,iBasis(Vec2(CosDegrees(orientationDegrees),SinDegrees(orientationDegrees)))
{
}

//////////////////////////////////////////////////////////////////////////
OBB2::OBB2( const AABB2& axisAlignedBox, float orientationDegrees /*= 0.f */ )
{
	center = axisAlignedBox.GetCenter();
	halfDimensions = axisAlignedBox.GetDimensions() * .5f;
	iBasis = Vec2( CosDegrees( orientationDegrees ), SinDegrees( orientationDegrees ) );
}

//////////////////////////////////////////////////////////////////////////
bool OBB2::IsPointInside( const Vec2& point ) const
{
	Vec2 centerToPoint = point - center;
	float iLength = GetProjectedLength2D( centerToPoint, iBasis );
	if( iLength >= halfDimensions.x || iLength <= -halfDimensions.x )
		return false;

	Vec2 jBasis( -iBasis.y, iBasis.x );
	float jLength = GetProjectedLength2D( centerToPoint, jBasis );
	if( jLength >= halfDimensions.y || jLength <= -halfDimensions.y )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
float OBB2::GetOrientationDegrees() const
{
	return Atan2Degrees( iBasis.y, iBasis.x );
}

//////////////////////////////////////////////////////////////////////////
float OBB2::GetOutterRadius() const
{
	return halfDimensions.GetLength();
}

//////////////////////////////////////////////////////////////////////////
float OBB2::GetInnerRadius() const
{
	return halfDimensions.x > halfDimensions.y ? halfDimensions.y : halfDimensions.x;
}

//////////////////////////////////////////////////////////////////////////
const Vec2 OBB2::GetNearestPoint( const Vec2& refPos ) const
{
	Vec2 centerToPoint = refPos - center;
	float iLength = GetProjectedLength2D( centerToPoint, iBasis );
	iLength = Clamp( iLength, -halfDimensions.x, halfDimensions.x );

	Vec2 jBasis( -iBasis.y, iBasis.x );
	float jLength = GetProjectedLength2D( centerToPoint, jBasis );
	jLength = Clamp( jLength, -halfDimensions.y, halfDimensions.y );

	return center+iLength*iBasis+jLength*jBasis;
}

//////////////////////////////////////////////////////////////////////////
const Vec2 OBB2::GetCenter() const
{
	return center;
}

//////////////////////////////////////////////////////////////////////////
const Vec2 OBB2::GetDimensions() const
{
	return halfDimensions * 2.f;
}

//////////////////////////////////////////////////////////////////////////
const Vec2 OBB2::GetIBasisNormal() const
{
	return iBasis;
}

//////////////////////////////////////////////////////////////////////////
const Vec2 OBB2::GetJBasisNormal() const
{
	return Vec2(-iBasis.y, iBasis.x );
}

//////////////////////////////////////////////////////////////////////////
const Vec2 OBB2::GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const
{
	Vec2 relativeHalfDim = (uvCoordsZeroToOne - Vec2( .5f, .5f ))*2.f;
	Vec2 relativePoint( relativeHalfDim.x * halfDimensions.x, relativeHalfDim.y * halfDimensions.y );
	return center + relativePoint.x*iBasis+relativePoint.y*GetJBasisNormal();
}

//////////////////////////////////////////////////////////////////////////
const Vec2 OBB2::GetUVForPoint( const Vec2& point ) const
{
	Vec2 relativePoint( point.x - center.x, point.y - center.y );

	float iLength = GetProjectedLength2D( relativePoint, iBasis );
	float iRelative = 0.f;
	if( halfDimensions.x != 0.f )
	{
		iRelative = iLength/halfDimensions.x;
	}

	Vec2 jBasis( -iBasis.y, iBasis.x );
	float jLength = GetProjectedLength2D( relativePoint, jBasis );
	float jRelative = 0.f;
	if( halfDimensions.y != 0.f )
	{
		jRelative = jLength / halfDimensions.y;
	}

	return Vec2( iRelative*0.5f + .5f, jRelative*0.5f + .5f );
}

//////////////////////////////////////////////////////////////////////////
void OBB2::GetCornerPositions( Vec2* out_fourPoints ) const
{
	Vec2 jBasis( -iBasis.y, iBasis.x );
	out_fourPoints[0] = center - halfDimensions.x * iBasis - halfDimensions.y * jBasis;
	out_fourPoints[1] = center + halfDimensions.x * iBasis - halfDimensions.y * jBasis;
	out_fourPoints[2] = center + halfDimensions.x * iBasis + halfDimensions.y * jBasis;
	out_fourPoints[3] = center - halfDimensions.x * iBasis + halfDimensions.y * jBasis;
}

//////////////////////////////////////////////////////////////////////////
void OBB2::Translate( const Vec2& translation )
{
	center += translation;
}

//////////////////////////////////////////////////////////////////////////
void OBB2::SetCenter( const Vec2& newCenter )
{
	center = newCenter;
}

//////////////////////////////////////////////////////////////////////////
void OBB2::SetDimensions( const Vec2& newDimensions )
{
	halfDimensions = newDimensions * .5f;
}

//////////////////////////////////////////////////////////////////////////
void OBB2::SetOrientationDegrees( float newAbsoluteOrientation )
{
	iBasis.x = CosDegrees( newAbsoluteOrientation );
	iBasis.y = SinDegrees( newAbsoluteOrientation );
}

//////////////////////////////////////////////////////////////////////////
void OBB2::RotateByDegrees( float relativeRotationDegrees )
{
	iBasis.RotateDegrees( relativeRotationDegrees );
}

//////////////////////////////////////////////////////////////////////////
void OBB2::StretchToIncludePoint( const Vec2& point )
{
	Vec2 relativePoint( point.x - center.x, point.y - center.y );

	float iLength = GetProjectedLength2D( relativePoint, iBasis );
	iLength = iLength < 0.f ? -iLength : iLength;
	halfDimensions.x = halfDimensions.x < iLength ? iLength : halfDimensions.x;
	
	Vec2 jBasis( -iBasis.y, iBasis.x );
	float jLength = GetProjectedLength2D( relativePoint, jBasis );
	jLength = jLength < 0.f ? -jLength : jLength;
	halfDimensions.y = halfDimensions.y < jLength ? jLength : halfDimensions.y;

}

//////////////////////////////////////////////////////////////////////////
void OBB2::Fix()
{
	//abs() half dimensions
	halfDimensions.x = halfDimensions.x < 0.f ? -halfDimensions.x : halfDimensions.x;
	halfDimensions.y = halfDimensions.y < 0.f ? -halfDimensions.y : halfDimensions.y;

	//normalize iBasis
	float iBasisLength = iBasis.GetLength();
	if( iBasisLength == 0.f )
	{
		iBasis = Vec2( 1.f, 0.f );
	}
	else if(iBasisLength!=1.f )
	{
		float factor = 1.f / iBasisLength;
		iBasis.x *= factor;
		iBasis.y *= factor;
	}

}

//////////////////////////////////////////////////////////////////////////
void OBB2::operator=( const OBB2& assignFrom )
{
	center = assignFrom.center;
	halfDimensions = assignFrom.halfDimensions;
	iBasis = assignFrom.iBasis;
}
