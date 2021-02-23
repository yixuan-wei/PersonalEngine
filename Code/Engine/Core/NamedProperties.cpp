#include "Engine/Core/NamedProperties.hpp"

//////////////////////////////////////////////////////////////////////////
NamedProperties::~NamedProperties()
{
    for (auto iter : m_keyValuePairs) {
        delete iter.second;
    }

    m_keyValuePairs.clear();
}


//////////////////////////////////////////////////////////////////////////
std::string NamedProperties::GetValue(std::string const& keyName, char const* val) const
{
    return GetValue<std::string>(keyName, val);
}

//////////////////////////////////////////////////////////////////////////
TypedPropertyBase* NamedProperties::FindInMap(std::string const& key) const
{
    auto iter = m_keyValuePairs.find(key);
    if (iter != m_keyValuePairs.end()) {
        return iter->second;
    }
    else {
        return nullptr;
    }
}


//////////////////////////////////////////////////////////////////////////
void NamedProperties::SetValue(std::string const& keyName, char const* value)
{
    SetValue<std::string>(keyName, value);
}