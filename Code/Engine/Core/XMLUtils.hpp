#pragma once

#include "ThirdParty/TinyXML2/tinyxml2.h"
#include "Engine/Core/StringUtils.hpp"
#include <string>
#include <vector>

struct Rgba8;
struct Vec2;
struct Vec3;
struct IntVec2;
struct AABB2;
struct IntRange;
struct FloatRange;

enum eCullMode : int;
enum eFillMode : int;
enum eCompareFunc : int;
enum eBlendMode : int;
enum eSamplerType : int;

typedef std::vector<int> Ints;
typedef tinyxml2::XMLElement XmlElement;
typedef tinyxml2::XMLAttribute XmlAttribute;
typedef tinyxml2::XMLDocument XmlDocument;
typedef tinyxml2::XMLError XmlError;

int ParseXmlAttribute( const XmlElement& element, const char* attributeName, int defaultValue );
char ParseXmlAttribute( const XmlElement& element, const char* attributeName, char defaultValue );
bool ParseXmlAttribute( const XmlElement& element, const char* attributeName, bool defaultValue );
float ParseXmlAttribute( const XmlElement& element, const char* attributeName, float defaultValue );
Rgba8 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Rgba8& defaultValue );
Vec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec2& defaultValue );
Vec3 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec3& defaultValue );
IntVec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntVec2& defaultValue );
AABB2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const AABB2& defaultValue );
IntRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntRange& defaultValue );
FloatRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const FloatRange& defaultValue );
Ints ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Ints& defaultValue );
std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const std::string& defaultValue );
std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const char* defaultValue );
Strings ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Strings& defaultValues );

eCullMode ParseXmlAttribute(const XmlElement& element, const char* attributeName, eCullMode defaultValue);
eFillMode ParseXmlAttribute(const XmlElement& element, const char* attributeName, eFillMode defaultValue);
eCompareFunc ParseXmlAttribute(const XmlElement& element, const char* attributeName, eCompareFunc defaultValue);
eBlendMode ParseXmlAttribute(const XmlElement& element, const char* attributeName, eBlendMode defaultValue);
eSamplerType ParseXmlAttribute(const XmlElement& element, const char* attributeName, eSamplerType defaultValue);