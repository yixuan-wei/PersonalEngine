#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include <stdarg.h>


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

//////////////////////////////////////////////////////////////////////////
bool IsNumber(std::string const& str)
{
	std::string::const_iterator it =str.begin();
	while (it != str.end() && std::isdigit(*it)) {
		++it;
	}
	return !str.empty() && it==str.end();
}

//////////////////////////////////////////////////////////////////////////
bool ParseEventRawString(std::string const& rawString, std::string& eventName, NamedProperties& eventParameters)
{
	Strings chunks = SplitEventRawString(rawString);
	eventName = chunks[0];
	chunks.erase(chunks.begin());

	for (size_t i = 0; i < chunks.size(); i++) {
		Strings tempPair = SplitStringOnDelimiter(chunks[i],'=');
		if (tempPair.size() == 1) {	//whole as a parameter value
			std::string& str = tempPair[0];
			if (IsNumber(str)) {
				g_theConsole->PrintError("Parameter name couldn't be numbers");
				return false;
			}

			eventParameters.SetValue(std::to_string(i), str);
		}
		else if (tempPair.size() == 2) {	
			std::string& str = tempPair[1];
			eventParameters.SetValue(tempPair[0], str);
			eventParameters.SetValue(std::to_string(i), str);
		}
		else {
			g_theConsole->PrintString(Rgba8::RED, "Illegal parameter input format");
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
Strings SplitEventRawString(std::string const& rawString)
{
	size_t splitPos = rawString.find(' ');
	Strings result;
	result.push_back(rawString.substr(0,splitPos));
	if (splitPos == std::string::npos) {
		return result;
	}

	std::string params = rawString.substr(splitPos+1);
	bool quoteStart = false;
	bool quoted = false;
	size_t lastStart = 0;
	for (size_t i = 0; i < params.size(); i++) {
		if (params[i] == ' ' && !quoteStart) {
			if (quoted) {
				result.push_back(params.substr(lastStart,i-lastStart-1));
			}
            else {
                result.push_back(params.substr(lastStart, i - lastStart));
			}
			lastStart=i+1;
			quoted = false;
		}
		else if (params[i] == '"') {
			quoteStart = !quoteStart;
			if (quoteStart) {
				lastStart++;
				quoted = true;
			}
		}
	}
	if (quoted) {
		result.push_back(params.substr(lastStart,params.size()-lastStart-1));
	}
    else {
        result.push_back(params.substr(lastStart));
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////
Strings SplitStringOnDelimiter( const std::string& originalString, char delimiterToSplitOn )
{
	Strings splitResult;
	std::string leftString = originalString;
	size_t lastSplitPos = 0;

	for( size_t splitPos = 0;; )
	{
		splitPos = leftString.find( delimiterToSplitOn,lastSplitPos );
		std::string subString( leftString, lastSplitPos, splitPos - lastSplitPos );
		splitResult.push_back( subString );

		if( splitPos == std::string::npos )
			break;
		
		lastSplitPos = splitPos + 1;
	}

	return splitResult;
}

//////////////////////////////////////////////////////////////////////////
std::string CombineStringsWithDelimiter(Strings const& strings, char delimiter)
{
	std::string result;
	int length = (int)strings.size();
	for (int idx = 0; idx < length-1; idx++)
	{
		result += strings[idx];
		result += delimiter;
	}
	if (length > 0) {
		result += strings[length - 1];
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////
void ClearEmptyStringInStrings(Strings& array)
{
	for (size_t i = 0; i < array.size();) {
		if (array[i].empty()) {
			array.erase(array.begin() + i);
		}
		else {
			i++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
std::string Trim( const std::string& s )
{
	std::string::const_iterator it = s.begin();
	while( it != s.end() && isspace( *it ) )
		it++;

	std::string::const_reverse_iterator rit = s.rbegin();
	while( rit.base() != it && isspace( *rit ) )
		rit++;

	return std::string( it, rit.base() );
}

//////////////////////////////////////////////////////////////////////////
std::string GetLowerCases( const std::string& s )
{
	std::string result = s;
	for( int chrIdx = 0; chrIdx < (int)s.length(); chrIdx++ )
	{
		result[chrIdx] = (char)std::tolower( s[chrIdx] );
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////
std::string GetFileNameFromPath(std::string const& path)
{
	Strings dirs = SplitStringOnDelimiter(path,'/');
	Strings newDirs = SplitStringOnDelimiter(dirs.back(),'\\');
	Strings fullNames = SplitStringOnDelimiter(newDirs.back(),'.');
	fullNames.pop_back();
	return CombineStringsWithDelimiter(fullNames,'.');
}

//////////////////////////////////////////////////////////////////////////
void Translate(std::string const& text, float* out)
{
	*out = (float)std::atof(text.c_str());
}

//////////////////////////////////////////////////////////////////////////
void Translate(std::string const& text, int* out)
{
	*out = std::atoi(text.c_str());
}

//////////////////////////////////////////////////////////////////////////
std::string ToString(int value)
{
	return Stringf("%i", value);
}

//////////////////////////////////////////////////////////////////////////
std::string ToString(float value)
{
	return Stringf("%f", value);
}

//////////////////////////////////////////////////////////////////////////
std::string ToString(char value)
{
	return Stringf("%c",value);
}

//////////////////////////////////////////////////////////////////////////
std::string ToString(bool value)
{
	std::string result = value ? "true" : "false";
	return result;
}

//////////////////////////////////////////////////////////////////////////
std::string ToString(Rgba8 const& value)
{
	return Stringf("%i,%i,%i,%i", value.r, value.g, value.b, value.a);
}

//////////////////////////////////////////////////////////////////////////
std::string ToString(Vec2 const& value)
{
	return Stringf("%f,%f", value.x, value.y);
}

//////////////////////////////////////////////////////////////////////////
std::string ToString(Vec3 const& value)
{
	return Stringf("%f,%f,%f", value.x, value.y, value.z);
}

//////////////////////////////////////////////////////////////////////////
std::string ToString(IntVec2 const& value)
{
	return Stringf("%i,%i", value.x, value.y);
}

//////////////////////////////////////////////////////////////////////////
std::string ToString(AABB2 const& value)
{
	return Stringf("%f,%f,%f,%f", value.mins.x, value.mins.y, value.maxs.x, value.maxs.y);
}

//////////////////////////////////////////////////////////////////////////
std::string ToString(IntRange const& value)
{
	return value.GetAsString();
}

//////////////////////////////////////////////////////////////////////////
std::string ToString(FloatRange const& value)
{
	return value.GetAsString();
}

//////////////////////////////////////////////////////////////////////////
std::string ToString(std::string const& value)
{
	return value;
}

//////////////////////////////////////////////////////////////////////////
std::string ToString(unsigned int value)
{
	return Stringf("%u", value);
}

//////////////////////////////////////////////////////////////////////////
int StringConvert(char const* str, int defaultValue)
{
	if (str == nullptr) {
		return defaultValue;
	}

	char* endptr;
	long value = std::strtol(str, &endptr, 10);
	if (endptr == str || *endptr != '\0') {
		g_theConsole->PrintError(Stringf("string conversion %s to int failed", str));
		return defaultValue;
	}
	else{
		return (int)value;
	}
}

//////////////////////////////////////////////////////////////////////////
float StringConvert(char const* str, float defaultValue)
{
	if (str == nullptr) {
		return defaultValue;
	}

	char* endptr;
	float value = std::strtof(str, &endptr);
	if (endptr == str || *endptr != '\0') {
		g_theConsole->PrintError(Stringf("string conversion %s to float failed", str));
		return defaultValue;
	}
	else {
		return value;
	}
}

//////////////////////////////////////////////////////////////////////////
bool StringConvert(char const* str, bool defaultValue)
{
	if (str == nullptr) {
		return defaultValue;
	}

    if (strcmp(str, "true") == 0)
    {
        return true;
    }
    else if (strcmp(str, "false") == 0)
    {
        return false;
    }
    else
    {
        g_theConsole->PrintError(Stringf("string conversion %s for bool failed, set to %s", 
					str, defaultValue?"true":"false"));
		return defaultValue;
    }
}

//////////////////////////////////////////////////////////////////////////
char StringConvert(char const* str, char defaultValue)
{
	if (str == nullptr) {
		return defaultValue;
	}

	if (str == nullptr || str[0] == '\0' || str[1]!='\0') {
		g_theConsole->PrintError(Stringf("string conversion %s to char failed", str));
		return defaultValue;
	}
	return str[0];
}

//////////////////////////////////////////////////////////////////////////
Rgba8 StringConvert(char const* str, Rgba8 const& defaultValue)
{
	Rgba8 value = defaultValue;
	value.SetFromText(str);
	return value;
}

//////////////////////////////////////////////////////////////////////////
Vec2 StringConvert(char const* str, Vec2 const& defaultValue)
{
	Vec2 value = defaultValue;
	value.SetFromText(str);
	return value;
}

//////////////////////////////////////////////////////////////////////////
Vec3 StringConvert(char const* str, Vec3 const& defaultValue)
{
	Vec3 value = defaultValue;
	value.SetFromText(str);
	return value;
}

//////////////////////////////////////////////////////////////////////////
IntVec2 StringConvert(char const* str, IntVec2 const& defaultValue)
{
	IntVec2 value = defaultValue;
	value.SetFromText(str);
	return value;
}

//////////////////////////////////////////////////////////////////////////
AABB2 StringConvert(char const* str, AABB2 const& defaultValue)
{
	AABB2 result = defaultValue;
	result.SetFromText(str);
	return result;
}

//////////////////////////////////////////////////////////////////////////
IntRange StringConvert(char const* str, IntRange const& defaultValue)
{
	IntRange result = defaultValue;
	result.SetFromText(str);
	return result;
}

//////////////////////////////////////////////////////////////////////////
FloatRange StringConvert(char const* str, FloatRange const& defaultValue)
{
	FloatRange result = defaultValue;
	result.SetFromText(str);
	return result;
}

//////////////////////////////////////////////////////////////////////////
std::string StringConvert(char const* str, std::string const& defaultValue)
{
	if (str == nullptr || str[0] == '\0') {
		return defaultValue;
	}

	return str;
}

//////////////////////////////////////////////////////////////////////////
unsigned int StringConvert(char const* str, unsigned int defaultValue)
{
    if (str == nullptr) {
        return defaultValue;
    }

    char* endptr;
    long value = std::strtol(str, &endptr, 10);
    if (endptr == str || *endptr != '\0') {
        g_theConsole->PrintError(Stringf("string conversion %s to int failed", str));
        return defaultValue;
    }
    else {
        return (unsigned int)value;
    }

}

//////////////////////////////////////////////////////////////////////////
double StringConvert(char const* str, double defaultValue)
{
    if (str == nullptr) {
        return defaultValue;
    }

    char* endptr;
    double value = std::strtod(str, &endptr);
    if (endptr == str || *endptr != '\0') {
        g_theConsole->PrintError(Stringf("string conversion %s to float failed", str));
        return defaultValue;
    }
    else {
        return value;
    }
}

