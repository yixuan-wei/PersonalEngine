#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Renderer/Sampler.hpp"

//////////////////////////////////////////////////////////////////////////
Vec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec2& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
    return StringConvert(attributeValueText, defaultValue);
}

//////////////////////////////////////////////////////////////////////////
Rgba8 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Rgba8& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
    return StringConvert(attributeValueText, defaultValue);
}

//////////////////////////////////////////////////////////////////////////
float ParseXmlAttribute( const XmlElement& element, const char* attributeName, float defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );	
	return StringConvert(attributeValueText,defaultValue);
}

//////////////////////////////////////////////////////////////////////////
bool ParseXmlAttribute( const XmlElement& element, const char* attributeName, bool defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );	
	return StringConvert(attributeValueText, defaultValue);
}

//////////////////////////////////////////////////////////////////////////
char ParseXmlAttribute( const XmlElement& element, const char* attributeName, char defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	return StringConvert(attributeValueText, defaultValue);
}

//////////////////////////////////////////////////////////////////////////
int ParseXmlAttribute( const XmlElement& element, const char* attributeName, int defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	return StringConvert( attributeValueText, defaultValue );
}

//////////////////////////////////////////////////////////////////////////
IntVec2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntVec2& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
    return StringConvert(attributeValueText, defaultValue);
}

//////////////////////////////////////////////////////////////////////////
std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const std::string& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
    return StringConvert(attributeValueText, defaultValue);
}

//////////////////////////////////////////////////////////////////////////
Vec3 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Vec3& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
    return StringConvert(attributeValueText, defaultValue);
}

//////////////////////////////////////////////////////////////////////////
AABB2 ParseXmlAttribute( const XmlElement& element, const char* attributeName, const AABB2& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
    return StringConvert(attributeValueText, defaultValue);
}

//////////////////////////////////////////////////////////////////////////
Ints ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Ints& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
	Ints result = defaultValue;
	if( attributeValueText )
	{
		result.clear();

		Strings values = SplitStringOnDelimiter( Stringf( attributeValueText ), ',' );
		for( int intID = 0; intID < (int)values.size(); intID++ )
		{
			result.push_back( StringConvert( values[intID].c_str(), 0 ) );
		}
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////
std::string ParseXmlAttribute( const XmlElement& element, const char* attributeName, const char* defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
    std::string defValue = defaultValue;
    return StringConvert(attributeValueText, defValue);
}

//////////////////////////////////////////////////////////////////////////
IntRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const IntRange& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
    return StringConvert(attributeValueText, defaultValue);
}

//////////////////////////////////////////////////////////////////////////
FloatRange ParseXmlAttribute( const XmlElement& element, const char* attributeName, const FloatRange& defaultValue )
{
	const char* attributeValueText = element.Attribute( attributeName );
    return StringConvert(attributeValueText, defaultValue);
}

//////////////////////////////////////////////////////////////////////////
Strings ParseXmlAttribute( const XmlElement& element, const char* attributeName, const Strings& defaultValues )
{
	const char* attributeValueText = element.Attribute( attributeName );
	Strings values = defaultValues;
	if( attributeValueText )
	{
		values.clear();
		values = SplitStringOnDelimiter( attributeValueText, ',' );
	}
	return values;
}

//////////////////////////////////////////////////////////////////////////
eCullMode ParseXmlAttribute(const XmlElement& element, const char* attributeName, eCullMode defaultValue)
{
    const char* attributeValueText = element.Attribute(attributeName);
    eCullMode value = defaultValue;
    if (attributeValueText)
    {
        if (strcmp(attributeValueText, "back") == 0)  {
            value = CULL_BACK;
        }
        else if (strcmp(attributeValueText, "front") == 0)  {
            value = CULL_FRONT;
        }
        else if (strcmp(attributeValueText, "none") == 0)  {
            value = CULL_NONE;
        }
        else  {
            ERROR_AND_DIE(Stringf("Invalid input %s for cullMode attribute in xml", attributeValueText));
        }
    }
    return value;
}

//////////////////////////////////////////////////////////////////////////
eFillMode ParseXmlAttribute(const XmlElement& element, const char* attributeName, eFillMode defaultValue)
{
    const char* attributeValueText = element.Attribute(attributeName);
    eFillMode value = defaultValue;
    if (attributeValueText)
    {
        if (strcmp(attributeValueText, "solid") == 0)  {
            value = FILL_SOLID;
        }
        else if (strcmp(attributeValueText, "wireframe") == 0)  {
            value = FILL_WIREFRAME;
        }
        else  {
            ERROR_AND_DIE(Stringf("Invalid input %s for fillMode attribute in xml", attributeValueText));
        }
    }
    return value;
}

//////////////////////////////////////////////////////////////////////////
eCompareFunc ParseXmlAttribute(const XmlElement& element, const char* attributeName, eCompareFunc defaultValue)
{
    const char* attributeValueText = element.Attribute(attributeName);
    eCompareFunc value = defaultValue;
    if (attributeValueText)
    {
        if (strcmp(attributeValueText, "never") == 0) {
            value = COMPARE_FUNC_NEVER;
        }
        else if (strcmp(attributeValueText, "always") == 0)  {
            value = COMPARE_FUNC_ALWAYS;
        }
        else if (strcmp(attributeValueText, "lequal") == 0) {
            value = COMPARE_FUNC_LEQUAL;
        }
        else if (strcmp(attributeValueText, "gequal") == 0) {
            value = COMPARE_FUNC_GEQUAL;
        }
        else if (strcmp(attributeValueText, "greater") == 0) {
            value = COMPARE_FUNC_GREATER;
        }
		else if (strcmp(attributeValueText, "equal") == 0) {
			value = COMPARE_FUNC_EQUAL;
		}
        else  {
            ERROR_AND_DIE(Stringf("Invalid input %s for compareFunc attribute in xml", attributeValueText));
        }
    }
    return value;
}

//////////////////////////////////////////////////////////////////////////
eBlendMode ParseXmlAttribute(const XmlElement& element, const char* attributeName, eBlendMode defaultValue)
{
    const char* attributeValueText = element.Attribute(attributeName);
    eBlendMode value = defaultValue;
    if (attributeValueText)
    {
        if (strcmp(attributeValueText, "alpha") == 0) {
            value = BLEND_ALPHA;
        }
        else if (strcmp(attributeValueText, "additive") == 0) {
            value = BLEND_ADDITIVE;
        }
        else if (strcmp(attributeValueText, "opaque") == 0) {
            value = BLEND_OPAQUE;
        }
        else {
            ERROR_AND_DIE(Stringf("Invalid input %s for compareFunc attribute in xml", attributeValueText));
        }
    }
    return value;
}

//////////////////////////////////////////////////////////////////////////
eSamplerType ParseXmlAttribute(const XmlElement& element, const char* attributeName, eSamplerType defaultValue)
{
    const char* attributeValueText = element.Attribute(attributeName);
    eSamplerType value = defaultValue;
    if (attributeValueText)
    {
        if (strcmp(attributeValueText, "linear_border") == 0) {
            return SAMPLER_LINEAR_BORDER;
        }
        else if (strcmp(attributeValueText, "point") == 0) {
            return SAMPLER_POINT;
        }
        else if (strcmp(attributeValueText, "bilinear") == 0) {
            return SAMPLER_BILINEAR;
        }
        else {
            ERROR_AND_DIE(Stringf("Invalid input %s for samplerType attribute in xml", attributeValueText));
        }
    }
    return value;
}
