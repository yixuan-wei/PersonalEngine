#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include <string>

//////////////////////////////////////////////////////////////////////////
FloatRange::FloatRange( float initStart, float initEnd )
	:minimum(initStart)
	,maximum(initEnd)
{
}

//////////////////////////////////////////////////////////////////////////
FloatRange::FloatRange( float minAndMax )
	:minimum(minAndMax)
	,maximum(minAndMax)
{
}

//////////////////////////////////////////////////////////////////////////
FloatRange::FloatRange( const char* asText )
{
	SetFromText( asText );
}

//////////////////////////////////////////////////////////////////////////
bool FloatRange::IsInRange( float value ) const
{
	if( value >= minimum && value <= maximum )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
bool FloatRange::DoesOverlap( const FloatRange& otherRange ) const
{
	if( otherRange.maximum >= minimum && otherRange.minimum <= maximum )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
std::string FloatRange::GetAsString() const
{
	if( minimum == maximum )
		return Stringf( "%f", minimum );
	else
		return Stringf( "%f~%f", minimum, maximum );
}

//////////////////////////////////////////////////////////////////////////
float FloatRange::GetRandomInRange( RandomNumberGenerator& rng ) const
{
	return rng.RollRandomFloatInRange( minimum, maximum );
}

//////////////////////////////////////////////////////////////////////////
void FloatRange::Set( float newMinimum, float newMaximum )
{
	minimum = newMinimum;
	maximum = newMaximum;
}

//////////////////////////////////////////////////////////////////////////
bool FloatRange::SetFromText( const char* text )
{
	if (text == nullptr) {
		return false;
	}

	Strings subStrings = SplitStringOnDelimiter( Stringf( text ), '~' );
	size_t subStringsSize = subStrings.size();
	if (subStringsSize > 2 || subStringsSize < 1) {
		g_theConsole->PrintError(Stringf("FloatRange can't construct from improper string \"%s\"", text) );
		return false;
	}

	if( subStringsSize == 1 )
	{
		minimum = maximum = StringConvert( subStrings[0].c_str(), 0.f );
	}
	else if( subStringsSize == 2 )
	{
        minimum = StringConvert(subStrings[0].c_str(), 0.f);
        maximum = StringConvert(subStrings[1].c_str(), 0.f);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool FloatRange::operator!=( const FloatRange& compare ) const
{
	if( compare.minimum != minimum || compare.maximum != maximum )
		return true;

	return false;
}
