#include <cmath>
#include <string>
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

const IntVec2 IntVec2::ZERO = IntVec2( 0, 0 );
const IntVec2 IntVec2::ONE = IntVec2( 1, 1 );

//////////////////////////////////////////////////////////////////////////
IntVec2::IntVec2( int initialX, int initialY )
	:x(initialX)
	,y(initialY)
{
}

//////////////////////////////////////////////////////////////////////////
IntVec2::IntVec2( const IntVec2& copyFrom )
	:x(copyFrom.x)
	,y(copyFrom.y)
{
}

//////////////////////////////////////////////////////////////////////////
void IntVec2::SetFromText( const char* text )
{
	if (text == nullptr) {
		return;
	}

	Strings subStrings = SplitStringOnDelimiter( Stringf( text ), ',' );
	if (subStrings.size() != 2) {
		g_theConsole->PrintError(Stringf("IntVec2 can't construct from improper string \"%s\"", text) );
		return;
	}

	x = StringConvert( subStrings[0].c_str(), 0 );
	y = StringConvert( subStrings[1].c_str(), 0 );
}

//////////////////////////////////////////////////////////////////////////
float IntVec2::GetLength() const
{
	float squaredLen = static_cast<float>(GetLengthSquared());
	return std::sqrtf( squaredLen );
}

//////////////////////////////////////////////////////////////////////////
int IntVec2::GetLengthSquared() const
{
	return x * x + y * y;
}

//////////////////////////////////////////////////////////////////////////
int IntVec2::GetTaxicabLength() const
{
	int newX = x > 0 ? x : -x;
	int newY = y > 0 ? y : -y;
	return newX + newY;
}

//////////////////////////////////////////////////////////////////////////
float IntVec2::GetOrientationRadians() const
{
	return std::atan2f( (float)y, (float)x );
}

//////////////////////////////////////////////////////////////////////////
float IntVec2::GetOrientationDegrees() const
{
	return Atan2Degrees( (float)y, (float)x );
}

//////////////////////////////////////////////////////////////////////////
const IntVec2 IntVec2::GetRotated90Degrees() const
{
	return IntVec2( -y, x );
}

//////////////////////////////////////////////////////////////////////////
const IntVec2 IntVec2::GetRotatedMinus90Degrees() const
{
	return IntVec2( y, -x );
}

//////////////////////////////////////////////////////////////////////////
void IntVec2::Rotate90Degrees()
{
	int temp = x;
	x = -y;
	y = temp;
}

//////////////////////////////////////////////////////////////////////////
void IntVec2::RotateMinus90Degrees()
{
	int temp = x;
	x = y;
	y = -temp;
}

//////////////////////////////////////////////////////////////////////////
const IntVec2 IntVec2::operator+( const IntVec2& vecToAdd ) const
{
	return IntVec2( x + vecToAdd.x, y + vecToAdd.y );
}

//////////////////////////////////////////////////////////////////////////
const IntVec2 IntVec2::operator-( const IntVec2& vecToSubtract ) const
{
	return IntVec2( x - vecToSubtract.x, y - vecToSubtract.y );
}

//////////////////////////////////////////////////////////////////////////
const IntVec2 IntVec2::operator-() const
{
	return IntVec2( -x, -y );
}

//////////////////////////////////////////////////////////////////////////
void IntVec2::operator=( const IntVec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}

//////////////////////////////////////////////////////////////////////////
void IntVec2::operator*=( const int uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}

//////////////////////////////////////////////////////////////////////////
const IntVec2 IntVec2::operator*( int uniformScale ) const
{
	return IntVec2( uniformScale * x, uniformScale * y );
}

//////////////////////////////////////////////////////////////////////////
const IntVec2 IntVec2::operator*( const IntVec2& vecToMultiply ) const
{
	return IntVec2( x * vecToMultiply.x, y * vecToMultiply.y );
}

//////////////////////////////////////////////////////////////////////////
void IntVec2::operator-=( const IntVec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}

//////////////////////////////////////////////////////////////////////////
void IntVec2::operator+=( const IntVec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}

//////////////////////////////////////////////////////////////////////////
bool IntVec2::operator!=( const IntVec2& compare ) const
{
	if( x != compare.x || y != compare.y )
		return true;
	else 
		return false;
}

//////////////////////////////////////////////////////////////////////////
bool IntVec2::operator==( const IntVec2& compare ) const
{
	if( x == compare.x && y == compare.y )
		return true;
	else 
		return false;
}

//////////////////////////////////////////////////////////////////////////
const IntVec2 operator*( int uniformScale, const IntVec2& vecToScale )
{
	return IntVec2( uniformScale * vecToScale.x, uniformScale * vecToScale.y );
}