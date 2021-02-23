#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/Vec3.hpp"

//////////////////////////////////////////////////////////////////////////
void NamedStrings::PopulateFromXmlElementAttributes( const XmlElement& element )
{
	const XmlAttribute* attribute = element.FirstAttribute();
	while( attribute != nullptr )
	{
		SetValue( attribute->Name(), attribute->Value() );
		attribute = attribute->Next();
	}
}

//////////////////////////////////////////////////////////////////////////
void NamedStrings::SetValue( const std::string& keyName, const std::string& newValue )
{
	m_keyValuePairs[keyName] = newValue;
}

//////////////////////////////////////////////////////////////////////////
bool NamedStrings::GetValue( const std::string& keyName, bool defaultValue ) const
{
	bool value = defaultValue;

	std::map<std::string, std::string>::const_iterator pair;
	pair = m_keyValuePairs.find( keyName );

	if( pair != m_keyValuePairs.end() )
	{
		std::string valueInPair = pair->second;
		if( valueInPair == "true" )
		{
			value = true;
		}
		else if( valueInPair == "false" )
		{
			value = false;
		}
		else
		{
			g_theConsole->PrintString(Rgba8::MAGENTA, Stringf( "bool namedstrings has wrong value: true/false\n" ) );
		}
	}
	else
	{
		std::string result = defaultValue ? "true" : "false";
		g_theConsole->PrintString(Rgba8::RED,Stringf( "key %s couldn't be found, set to %s", 
			keyName.c_str(), result.c_str() ) );
	}

	return value;
}

//////////////////////////////////////////////////////////////////////////
int NamedStrings::GetValue( const std::string& keyName, int defaultValue ) const
{
	int value = defaultValue;

	std::map<std::string, std::string>::const_iterator pair;
	pair = m_keyValuePairs.find( keyName );

	if( pair != m_keyValuePairs.end() )
	{
		std::string valueInPair = pair->second;
		value = std::atoi( valueInPair.c_str() );
	}
	else
	{
		g_theConsole->PrintString(Rgba8::RED, Stringf( "key %s couldn't be found, set to %i", 
			keyName.c_str(), defaultValue ) );
	}

	return value;
}

//////////////////////////////////////////////////////////////////////////
float NamedStrings::GetValue( const std::string& keyName, float defaultValue ) const
{
	float value = defaultValue;

	std::map<std::string, std::string>::const_iterator pair;
	pair = m_keyValuePairs.find( keyName );

	if( pair != m_keyValuePairs.end() )
	{
		std::string valueInPair = pair->second;
		value = (float)std::atof( valueInPair.c_str() );
	}
	else
	{
		g_theConsole->PrintString(Rgba8::RED, Stringf( "key %s couldn't be found, set to %.2f", 
			keyName.c_str(), defaultValue ) );
	}

	return value;
}

//////////////////////////////////////////////////////////////////////////
std::string NamedStrings::GetValue( const std::string& keyName, std::string defaultValue ) const
{
	std::string value = defaultValue;

	std::map<std::string, std::string>::const_iterator pair;
	pair = m_keyValuePairs.find( keyName );

	if( pair != m_keyValuePairs.end() )
	{
		value = pair->second;
	}
	else
	{
		g_theConsole->PrintString(Rgba8::RED, Stringf( "key %s couldn't be found, set to %s", 
			keyName.c_str(), defaultValue.c_str() ) );
	}

	return value;
}

//////////////////////////////////////////////////////////////////////////
std::string NamedStrings::GetValue( const std::string& keyName, const char* defaultValue ) const
{
	std::string value = defaultValue;

	std::map<std::string, std::string>::const_iterator pair;
	pair = m_keyValuePairs.find( keyName );

	if( pair != m_keyValuePairs.end() )
	{
		value = pair->second;
	}
	else
	{
		g_theConsole->PrintString(Rgba8::RED, Stringf( "key %s couldn't be found, set to %s", 
			keyName.c_str(), defaultValue ) );
	}

	return value;
}

//////////////////////////////////////////////////////////////////////////
Rgba8 NamedStrings::GetValue( const std::string& keyName, const Rgba8& defaultValue ) const
{
	Rgba8 value = defaultValue;

	std::map<std::string, std::string>::const_iterator pair;
	pair = m_keyValuePairs.find( keyName );

	if( pair != m_keyValuePairs.end() )
	{
		std::string valueInPair = pair->second;
		value.SetFromText( valueInPair.c_str() );
	}
	else
	{
		g_theConsole->PrintString(Rgba8::RED, Stringf( "key %s couldn't be found, set to (%i, %i, %i, %i)", 
			keyName.c_str(), (int)defaultValue.r, (int)defaultValue.g, (int)defaultValue.b, (int)defaultValue.a ) );
	}

	return value;
}

//////////////////////////////////////////////////////////////////////////
Vec2 NamedStrings::GetValue( const std::string& keyName, const Vec2& defaultValue ) const
{
	Vec2 value = defaultValue;

	std::map<std::string, std::string>::const_iterator pair;
	pair = m_keyValuePairs.find( keyName );

	if( pair != m_keyValuePairs.end() )
	{
		std::string valueInPair = pair->second;
		value.SetFromText( valueInPair.c_str() );
	}
	else
	{
		g_theConsole->PrintString(Rgba8::RED, Stringf( "key %s couldn't be found, set to (%.2f, %.2f)", 
			keyName.c_str(), defaultValue.x, defaultValue.y ) );
	}

	return value;
}

//////////////////////////////////////////////////////////////////////////
IntVec2 NamedStrings::GetValue( const std::string& keyName, const IntVec2& defaultValue ) const
{
	IntVec2 value = defaultValue;

	std::map<std::string, std::string>::const_iterator pair;
	pair = m_keyValuePairs.find( keyName );

	if( pair != m_keyValuePairs.end() )
	{
		std::string valueInPair = pair->second;
		value.SetFromText( valueInPair.c_str() );
	}
	else
	{
		g_theConsole->PrintString(Rgba8::RED, Stringf( "key %s couldn't be found, set to (%i, %i)", 
			keyName.c_str(), defaultValue.x, defaultValue.y ) );
	}

	return value;
}

//////////////////////////////////////////////////////////////////////////
FloatRange NamedStrings::GetValue( const std::string& keyName, const FloatRange& defaultValue ) const
{
	FloatRange value = defaultValue;

	std::map<std::string, std::string>::const_iterator pair;
	pair = m_keyValuePairs.find( keyName );

	if( pair != m_keyValuePairs.end() )
	{
		std::string valueInPair = pair->second;
		value.SetFromText( valueInPair.c_str() );
	}
	else
	{
		g_theConsole->PrintString(Rgba8::RED, Stringf( "key %s couldn't be found, set to [%.2f, %.2f]", 
			keyName.c_str(), defaultValue.minimum, defaultValue.maximum ) );
	}

	return value;
}

//////////////////////////////////////////////////////////////////////////
IntRange NamedStrings::GetValue( const std::string& keyName, const IntRange& defaultValue ) const
{
	IntRange value = defaultValue;

	std::map<std::string, std::string>::const_iterator pair;
	pair = m_keyValuePairs.find( keyName );

	if( pair != m_keyValuePairs.end() )
	{
		std::string valueInPair = pair->second;
		value.SetFromText( valueInPair.c_str() );
	}
	else
	{
		g_theConsole->PrintString(Rgba8::MAGENTA, Stringf( "key %s couldn't be found, set to [%i, %i]", 
			keyName.c_str(), defaultValue.minimum, defaultValue.maximum ) );
	}

	return value;
}

//////////////////////////////////////////////////////////////////////////
Vec3 NamedStrings::GetValue(const std::string& keyName, const Vec3& defaultValue) const
{
    Vec3 value = defaultValue;

    std::map<std::string, std::string>::const_iterator pair;
    pair = m_keyValuePairs.find(keyName);

    if (pair != m_keyValuePairs.end())
    {
        std::string valueInPair = pair->second;
        value.SetFromText(valueInPair.c_str());
    }
    else
    {
        g_theConsole->PrintString(Rgba8::MAGENTA, Stringf("key %s couldn't be found, set to (%.2f, %.2f, %.2f)", 
			keyName.c_str(), defaultValue.x, defaultValue.y, defaultValue.z));
    }

    return value;
}

//////////////////////////////////////////////////////////////////////////
void* NamedStrings::GetValue(const std::string& keyName) const
{
	void* value = nullptr;

    std::map<std::string, std::string>::const_iterator pair;
    pair = m_keyValuePairs.find(keyName);

    if (pair != m_keyValuePairs.end())
    {
        std::string valueInPair = pair->second;
        sscanf_s(valueInPair.c_str(),"%p", &value);
    }
    else
    {
        g_theConsole->PrintString(Rgba8::MAGENTA, Stringf("key %s couldn't be found, set to nullptr", keyName.c_str()));
    }

    return value;
}
