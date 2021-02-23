#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"

//////////////////////////////////////////////////////////////////////////
Vec4::Vec4( const Vec4& copyFrom )
	:x(copyFrom.x)
	,y(copyFrom.y)
	,z(copyFrom.z)
	,w(copyFrom.w)
{
}

//////////////////////////////////////////////////////////////////////////
Vec4::Vec4( float initialX, float initialY, float initialZ, float initialW )
	:x(initialX)
	,y(initialY)
	,z(initialZ)
	,w(initialW)
{
}

//////////////////////////////////////////////////////////////////////////
Vec4::Vec4(Vec3 const& copyFrom, float initialW /*= 0.f*/)
	:x(copyFrom.x)
	,y(copyFrom.y)
	,z(copyFrom.z)
	,w(initialW)
{
}

//////////////////////////////////////////////////////////////////////////
float Vec4::GetLengthSquared() const
{
	return x * x + y * y + z * z + w * w;
}

//////////////////////////////////////////////////////////////////////////
Vec3 Vec4::GetXYZ() const
{
	return Vec3(x, y, z);
}

//////////////////////////////////////////////////////////////////////////
Vec2 Vec4::GetXY() const
{
	return Vec2(x, y);
}

//////////////////////////////////////////////////////////////////////////
bool Vec4::operator!=( const Vec4& compare ) const
{
	if( x != compare.x || y != compare.y || z != compare.z || w != compare.w )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
bool Vec4::operator==( const Vec4& compare ) const
{
	if( x == compare.x && y == compare.y && z == compare.z && w == compare.w )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
const Vec4 Vec4::operator+( const Vec4& vecToAdd ) const
{
	return Vec4( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z, w + vecToAdd.w );
}

//////////////////////////////////////////////////////////////////////////
const Vec4 Vec4::operator-( const Vec4& vecToSubstract ) const
{
	return Vec4( x - vecToSubstract.x, y - vecToSubstract.y, z - vecToSubstract.z, w - vecToSubstract.w );
}

//////////////////////////////////////////////////////////////////////////
const Vec4 Vec4::operator-() const
{
	return Vec4( -x, -y, -z, -w );
}

//////////////////////////////////////////////////////////////////////////
void Vec4::operator/=( const float uniformDivisor )
{
	if( uniformDivisor == 0.f )
		return;

	float scale = 1.f / uniformDivisor;
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;
}

//////////////////////////////////////////////////////////////////////////
void Vec4::operator=( const Vec4& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}

//////////////////////////////////////////////////////////////////////////
void Vec4::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}

//////////////////////////////////////////////////////////////////////////
const Vec4 Vec4::operator*( float uniformScale ) const
{
	return Vec4( x * uniformScale, y * uniformScale, z * uniformScale, w * uniformScale );
}

//////////////////////////////////////////////////////////////////////////
const Vec4 Vec4::operator*( const Vec4& vecToMultiply ) const
{
	return Vec4(x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z, w * vecToMultiply.w);
}

//////////////////////////////////////////////////////////////////////////
const Vec4 Vec4::operator/( float inverseScale ) const
{
	if( inverseScale == 0.f )
		return Vec4( x, y, z, w );

	float scale = 1.f / inverseScale;
	return Vec4( x * scale, y * scale, z * scale, w * scale );
}

//////////////////////////////////////////////////////////////////////////
void Vec4::operator-=( const Vec4& vecToSubstract )
{
	x -= vecToSubstract.x;
	y -= vecToSubstract.y;
	z -= vecToSubstract.z;
	w -= vecToSubstract.w;
}

//////////////////////////////////////////////////////////////////////////
void Vec4::operator+=( const Vec4& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
	w += vecToAdd.w;
}

//////////////////////////////////////////////////////////////////////////
const Vec4 operator*( float uniformScale, const Vec4& vecToScale )
{
	return Vec4( uniformScale * vecToScale.x, uniformScale * vecToScale.y, uniformScale * vecToScale.z, uniformScale * vecToScale.w );
}