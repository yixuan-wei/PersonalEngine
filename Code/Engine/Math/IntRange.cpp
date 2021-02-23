#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include <string>

//////////////////////////////////////////////////////////////////////////
IntRange::IntRange( int initStart, int initEnd )
	:minimum(initStart)
	,maximum(initEnd)
{
}

//////////////////////////////////////////////////////////////////////////
IntRange::IntRange( int minAndMax )
	:minimum(minAndMax)
	,maximum(minAndMax)
{
}

//////////////////////////////////////////////////////////////////////////
IntRange::IntRange( const char* asText )
{
	SetFromText( asText );
}

//////////////////////////////////////////////////////////////////////////
bool IntRange::IsInRange( int value ) const
{
	if( value >= minimum && value <= maximum )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
bool IntRange::DoesOverlap( const IntRange& otherRange ) const
{
	if( minimum >= otherRange.maximum && maximum <= otherRange.minimum )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
std::string IntRange::GetAsString() const
{
	if( minimum == maximum )
		return Stringf( "%i", minimum );
	else
		return Stringf( "%i~%i", minimum, maximum );
}

//////////////////////////////////////////////////////////////////////////
int IntRange::GetRandomInRange( RandomNumberGenerator& rng ) const
{
	return rng.RollRandomIntInRange( minimum, maximum );
}

//////////////////////////////////////////////////////////////////////////
void IntRange::Set( int newMinimum, int newMaximum )
{
	minimum = newMinimum;
	maximum = newMaximum;
}

//////////////////////////////////////////////////////////////////////////
bool IntRange::SetFromText( const char* text )
{
	if (text == nullptr) {
		return false;
	}

	Strings subStrings = SplitStringOnDelimiter( Stringf( text ), '~' );
	size_t subStringsSize = subStrings.size();
	if (subStringsSize < 1 || subStringsSize>2) {
		g_theConsole->PrintError(Stringf("IntRange can't construct from improper string \"%s\"", text) );
		return false;
	}

	if( subStringsSize == 1 )
	{
		minimum = maximum = StringConvert( subStrings[0].c_str(), 0 );
	}
	else if( subStringsSize == 2 )
	{
		minimum = StringConvert( subStrings[0].c_str(), 0 );
		maximum = StringConvert( subStrings[1].c_str(), 0 );
	}
	return true;
}
