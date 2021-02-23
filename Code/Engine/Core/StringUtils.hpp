#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

struct Rgba8;
struct Vec2;
struct Vec3;
struct IntVec2;
struct AABB2;
struct IntRange;
struct FloatRange;
class NamedProperties;

typedef std::vector<std::string> Strings;

//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );

bool IsNumber(std::string const& str);

bool ParseEventRawString(std::string const& rawString, std::string& eventName, NamedProperties& eventParameters);
Strings SplitEventRawString(std::string const& rawString);

Strings SplitStringOnDelimiter( const std::string& originalString, char delimiterToSplitOn );
std::string CombineStringsWithDelimiter(Strings const& strings, char delimiter);

void ClearEmptyStringInStrings(Strings& array);

std::string Trim( const std::string& s );
std::string GetLowerCases( const std::string& s );

std::string GetFileNameFromPath(std::string const& path);

void Translate(std::string const& text, float* out);
void Translate(std::string const& text, int* out);

std::string ToString(unsigned int value);
std::string ToString(int value);
std::string ToString(float value);
std::string ToString(char value);
std::string ToString(bool value);
std::string ToString(Rgba8 const& value);
std::string ToString(Vec2 const& value);
std::string ToString(Vec3 const& value);
std::string ToString(IntVec2 const& value);
std::string ToString(AABB2 const& value);
std::string ToString(IntRange const& value);
std::string ToString(FloatRange const& value);
std::string ToString(std::string const& value);

unsigned int StringConvert(char const* str, unsigned int defaultValue);
int         StringConvert(char const* str, int defaultValue);
float       StringConvert(char const* str, float defaultValue);
double      StringConvert(char const* str, double defaultValue);
bool        StringConvert(char const* str, bool defaultValue);
char        StringConvert(char const* str, char defaultValue);
Rgba8       StringConvert(char const* str, Rgba8 const& defaultValue);
Vec2        StringConvert(char const* str, Vec2 const& defaultValue);
Vec3        StringConvert(char const* str, Vec3 const& defaultValue);
IntVec2     StringConvert(char const* str, IntVec2 const& defaultValue);
AABB2       StringConvert(char const* str, AABB2 const& defaultValue);
IntRange    StringConvert(char const* str, IntRange const& defaultValue);
FloatRange  StringConvert(char const* str, FloatRange const& defaultValue);
std::string StringConvert(char const* str, std::string const& defaultValue);
template<typename T>
T           StringConvert(char const* str, T const& defaultValue);


//////////////////////////////////////////////////////////////////////////
// class definitions
//////////////////////////////////////////////////////////////////////////
template<typename T>
T StringConvert(char const* str, T const& defaultValue)
{
    if (str == nullptr) {
        return defaultValue;
    }

    T result = reinterpret_cast<T>((char*)str);
    if (result) {
        return result;
    }

    return defaultValue;
}
