#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <cmath>
#include <string>

const Vec3 Vec3::ZERO = Vec3( 0.f, 0.f, 0.f );

//////////////////////////////////////////////////////////////////////////
Vec3::Vec3(const Vec3& copyFrom)
	: x( copyFrom.x )
	, y( copyFrom.y )
	, z( copyFrom.z )
{}

//////////////////////////////////////////////////////////////////////////
Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{}

//////////////////////////////////////////////////////////////////////////
Vec3::Vec3(Vec2 const& copyFrom, float initialZ /*= 0.f*/)
	:x(copyFrom.x)
	,y(copyFrom.y)
	,z(initialZ)
{
}

//////////////////////////////////////////////////////////////////////////
Vec3::Vec3(float uniformValue)
	:x(uniformValue)
	,y(uniformValue)
	,z(uniformValue)
{
}

//////////////////////////////////////////////////////////////////////////
void Vec3::SetFromText( const char* text )
{
	if (text == nullptr) {
		return;
	}

	Strings subStrings = SplitStringOnDelimiter( Stringf( text ), ',' );
	if (subStrings.size() != 3) {
		g_theConsole->PrintError(Stringf("Vec3 can't construct from improper string \"%s\"", text) );
		return;
	}

	x = StringConvert(subStrings[0].c_str(), 0.f);
	y = StringConvert(subStrings[1].c_str(), 0.f);
	z = StringConvert(subStrings[2].c_str(), 0.f);
}

//////////////////////////////////////////////////////////////////////////
void Vec3::SetLength(float length)
{
	float oldLength = GetLength();
	if (oldLength == 0.f) {
		return;
	}

	float factor = length / oldLength;
	x *= factor;
	y *= factor;
	z *= factor;
}

//////////////////////////////////////////////////////////////////////////
void Vec3::Normalize()
{
	SetLength(1.f);
}

//////////////////////////////////////////////////////////////////////////
float Vec3::GetLength() const
{
	return std::sqrtf(GetLengthSquared());
}

//////////////////////////////////////////////////////////////////////////
float Vec3::GetLengthXY() const
{
	return std::sqrtf(GetLengthXYSquared());
}

//////////////////////////////////////////////////////////////////////////
float Vec3::GetLengthSquared() const
{
	return x*x + y*y + z*z;
}

//////////////////////////////////////////////////////////////////////////
float Vec3::GetLengthXYSquared() const
{
	return x*x + y*y;
}

//////////////////////////////////////////////////////////////////////////
float Vec3::GetAngleAboutZRadians() const
{
	return std::atan2f( y, x );
}

//////////////////////////////////////////////////////////////////////////
float Vec3::GetAngleAboutZDegrees() const
{
	return Atan2Degrees( y, x );
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Vec3::GetRotatedAboutZRadians( float deltaRadians ) const
{
	float newRadians = GetAngleAboutZRadians() + deltaRadians;
	float lengthXY = GetLengthXY();
	return Vec3( lengthXY*std::cosf(newRadians), lengthXY*std::sinf(newRadians), z);
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Vec3::GetRotatedAboutZDegrees( float deltaDegrees ) const
{
	float newDegrees = GetAngleAboutZDegrees() + deltaDegrees;
	float lengthXY = GetLengthXY();
	return Vec3( lengthXY*CosDegrees(newDegrees), lengthXY*SinDegrees(newDegrees), z );
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Vec3::GetClamped( float maxLength ) const
{
	float length = GetLength();
	if( length!=0.f && length > maxLength )
	{
		float factor = maxLength / length;
		return Vec3( x * factor, y * factor, z * factor );
	}
	else
	{
		return Vec3( x, y, z );
	}
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Vec3::GetNormalized() const
{
	float length = GetLength();
	if( length!=0.f )
	{
		float lengthFactor = 1.f / length;
		return Vec3( x*lengthFactor, y*lengthFactor, z*lengthFactor );
	}
	else
	{
		return Vec3( 0.f, 0.f, 0.f );
	}
}

//////////////////////////////////////////////////////////////////////////
const Vec2 Vec3::GetXY() const
{
	return Vec2(x, y);
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Vec3::operator+( const Vec3& vecToAdd ) const
{
	return Vec3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z );
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Vec3::operator-( const Vec3& vecToSubtract ) const
{
	return Vec3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z );
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Vec3::operator-() const
{
	return Vec3( -x, -y, -z );
}

//////////////////////////////////////////////////////////////////////////
void Vec3::operator=( const Vec3& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Vec3::operator*( float uniformScale ) const
{
	return Vec3( x * uniformScale, y * uniformScale, z * uniformScale );
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Vec3::operator*( const Vec3& vecToMultiply ) const
{
	return Vec3( x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z );
}

//////////////////////////////////////////////////////////////////////////
const Vec3 Vec3::operator/( float inverseScale ) const
{
	if( inverseScale!=0.f )
	{
		float multiplier = 1.f / inverseScale;
		return Vec3( x * multiplier, y * multiplier, z * multiplier );
	}
	else
	{
		ERROR_RECOVERABLE( Stringf( "Vec3::/ is divided by 0, return" ) );
		return Vec3( x, y, z );
	}
}

//////////////////////////////////////////////////////////////////////////
void Vec3::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}

//////////////////////////////////////////////////////////////////////////
void Vec3::operator/=( const float uniformDivisor )
{
	if( uniformDivisor!=0.f )
	{
		float multiplier = 1.f / uniformDivisor;
		x *= multiplier;
		y *= multiplier;
		z *= multiplier;
	}	
	else
	{
		ERROR_RECOVERABLE( Stringf( "Vec3::/= is divided by 0, return" ) );
	}
}

//////////////////////////////////////////////////////////////////////////
void Vec3::operator-=( const Vec3& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}

//////////////////////////////////////////////////////////////////////////
void Vec3::operator+=( const Vec3& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}

//////////////////////////////////////////////////////////////////////////
bool Vec3::operator!=( const Vec3& compare ) const
{
	return ( x != compare.x || y != compare.y || z != compare.z );
}

//////////////////////////////////////////////////////////////////////////
bool Vec3::operator==( const Vec3& compare ) const
{
	return ( x == compare.x && y == compare.y && z == compare.z );
}

//////////////////////////////////////////////////////////////////////////
const Vec3 operator*( float uniformScale, const Vec3& vecToScale )
{
	return Vec3( uniformScale * vecToScale.x, uniformScale * vecToScale.y, uniformScale * vecToScale.z );
}