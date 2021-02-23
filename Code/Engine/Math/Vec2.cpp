#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <cmath>
#include <string>

const Vec2 Vec2::ZERO = Vec2( 0.f, 0.f );
const Vec2 Vec2::ONE = Vec2( 1.f, 1.f );

//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
	: x( copy.x )
	, y( copy.y )
{}



//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{}

//////////////////////////////////////////////////////////////////////////
void Vec2::SetFromText( const char* text )
{
	if (text == nullptr) {
		return;
	}

	Strings subStrings = SplitStringOnDelimiter( Stringf( text ), ',' );
	if (subStrings.size() != 2) {
		g_theConsole->PrintError(Stringf("Vec2 can't construct from improper string \"%s\"", text) );
		return;
	}

	x = StringConvert(subStrings[0].c_str(), 0.f);
	y = StringConvert(subStrings[1].c_str(), 0.f);
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const
{
	return Vec2( x + vecToAdd.x, y + vecToAdd.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	return Vec2( x - vecToSubtract.x, y - vecToSubtract.y );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-() const
{
	return Vec2( -x, -y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2( uniformScale * x, uniformScale * y );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( const Vec2& vecToMultiply ) const
{
	return Vec2( x * vecToMultiply.x, y * vecToMultiply.y ); 
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	if( inverseScale!=0.f )
	{
		float multiplier = 1.f / inverseScale;
		return Vec2( x * multiplier, y * multiplier );
	}
	else
	{
		ERROR_RECOVERABLE( Stringf( "Vec2 / is divided by 0, return" ) );
		return Vec2( x, y );
	}
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Vec2::MakeFromPolarRadians( float directionRadians, float length /*= 1.f */ )
{
	float newX = length * cosf(directionRadians);
	float newY = length * sinf(directionRadians);
	return Vec2( newX, newY );
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Vec2::MakeFromPolarDegrees( float directionDegrees, float length /*= 1.f */ )
{
	float newX = length * CosDegrees(directionDegrees);
	float newY = length * SinDegrees(directionDegrees);
	return Vec2( newX, newY );
}

//////////////////////////////////////////////////////////////////////////
float Vec2::GetLength() const
{
	return std::sqrtf( GetLengthSquared() );
}

//////////////////////////////////////////////////////////////////////////
float Vec2::GetLengthSquared() const
{
	return x*x + y*y;
}

//////////////////////////////////////////////////////////////////////////
float Vec2::GetAngleRadians() const
{
	return std::atan2f( y, x );
}

//////////////////////////////////////////////////////////////////////////
float Vec2::GetAngleDegrees() const
{
	return Atan2Degrees( y, x );
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Vec2::GetRotated90Degrees() const
{
	return Vec2( -y, x );
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Vec2::GetRotatedMinus90Degrees() const
{
	return Vec2( y, -x );
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Vec2::GetRotatedRadians( float deltaRadians ) const
{
	Vec2 iLocal = Vec2::MakeFromPolarRadians( deltaRadians );
	Vec2 jLocal( -iLocal.y, iLocal.x );
	return x*iLocal+y*jLocal;
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Vec2::GetRotatedDegrees( float deltaDegrees ) const
{
	Vec2 iLocal = Vec2::MakeFromPolarDegrees( deltaDegrees );
	Vec2 jLocal( -iLocal.y, iLocal.x );
	return x * iLocal + y * jLocal;
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Vec2::GetClamped( float maxLength ) const // Clamp the Vec2 if longer than given maxLength
{
	float lengthSquared = GetLengthSquared();
	if( lengthSquared == 0.f )
		return Vec2::ZERO;

	float fractionSquared = (maxLength * maxLength)/lengthSquared;
	if( fractionSquared<1.f )
	{
		float fraction = std::sqrtf( fractionSquared );
		return Vec2( x, y ) * fraction;
	}
	else
	{
		return Vec2( x, y );
	}
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Vec2::GetNormalized() const
{
	float length = GetLength();
	if( length == 0.f )
		return Vec2::ZERO;

	float factor = 1.f / length;
	return Vec2(x * factor, y * factor);
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Vec2::GetReflected( const Vec2& normal ) const
{
	Vec2 projectedOnNormal = GetProjectedOnto2D( *this, normal );
	Vec2 aperture = *this - projectedOnNormal;
	return aperture - projectedOnNormal;
}

//////////////////////////////////////////////////////////////////////////
void Vec2::SetAngleRadians( float newAngleRadians )
{
	float length = GetLength();
	x = length * std::cosf(newAngleRadians);
	y = length * std::sinf(newAngleRadians);
}

//////////////////////////////////////////////////////////////////////////
void Vec2::SetAngleDegrees( float newAngleDegrees )
{
	float length = GetLength();
	x = length * CosDegrees(newAngleDegrees);
	y = length * SinDegrees(newAngleDegrees);
}

//////////////////////////////////////////////////////////////////////////
void Vec2::SetPolarRadians( float newAngleRadians, float newLength )
{
	x = newLength * std::cosf(newAngleRadians);
	y = newLength * std::sinf(newAngleRadians);
}

//////////////////////////////////////////////////////////////////////////
void Vec2::SetPolarDegrees( float newAngleDegrees, float newLength )
{
	x = newLength * CosDegrees(newAngleDegrees);
	y = newLength * SinDegrees(newAngleDegrees);
}

//////////////////////////////////////////////////////////////////////////
void Vec2::Reflect( const Vec2& normal )
{
	Vec2 reflected = GetReflected( normal );
	x = reflected.x;
	y = reflected.y;
}

//////////////////////////////////////////////////////////////////////////
void Vec2::Rotate90Degrees()
{
	float temp = x;
	x = -y;
	y = temp;
}

//////////////////////////////////////////////////////////////////////////
void Vec2::RotateMinus90Degrees()
{
	float temp = x;
	x = y;
	y = -temp;
}

//////////////////////////////////////////////////////////////////////////
void Vec2::RotateRadians( float deltaRadians )
{
	*this = GetRotatedRadians( deltaRadians );
}

//////////////////////////////////////////////////////////////////////////
void Vec2::RotateDegrees( float deltaDegrees )
{
	*this = GetRotatedDegrees( deltaDegrees );
}

//////////////////////////////////////////////////////////////////////////
void Vec2::SetLength( float newLength )
{
	float oldLength = GetLength();
	if( oldLength != 0.f )
	{
		float scale = newLength / oldLength;
		x *= scale;
		y *= scale;
	}
}

void Vec2::ClampLength( float maxLength )
{
	float lengthSquared = GetLengthSquared();
	if( lengthSquared > maxLength*maxLength )
	{
		float scale = maxLength / std::sqrtf( lengthSquared );
		x *= scale;
		y *= scale;
	}
}

void Vec2::Normalize()
{
	float radius = GetLength();
	if( radius!=0.f )
	{
		float scale = 1.f / radius;
		x *= scale;
		y *= scale;
	}
}

float Vec2::NormalizeAndGetPreviousLength()
{
	float prevLength = GetLength();
	Normalize();
	return prevLength;
}

//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	if( uniformDivisor!=0.f )
	{
		float uniformMultiplier = 1.f / uniformDivisor;
		x *= uniformMultiplier;
		y *= uniformMultiplier;
	}	
	else
	{
		ERROR_RECOVERABLE( Stringf( "Vec2 /= is divided by 0, return" ) );
	}
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	return Vec2( uniformScale * vecToScale.x, uniformScale * vecToScale.y );
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
	return ( x == compare.x && y == compare.y );
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
	return ( x != compare.x || y != compare.y );
}

