#pragma once

#include "Engine/Core/StringUtils.hpp"
#include <string>
#include <map>

//////////////////////////////////////////////////////////////////////////
class TypedPropertyBase
{
public:
    virtual ~TypedPropertyBase() {}

    virtual std::string GetAsString() const = 0;
    virtual void const* GetUniqueID() const = 0;
    
    template<typename T>
    bool Is() const;
};

//////////////////////////////////////////////////////////////////////////
template<typename VALUE_TYPE>
class TypedProperty :public TypedPropertyBase
{
public:
    virtual std::string GetAsString() const final { return ToString(m_value).c_str(); }
    virtual void const* GetUniqueID() const final { return StaticUniqueID(); }

public:
    VALUE_TYPE m_value;

public:
    static void const* const StaticUniqueID();
    
};

//////////////////////////////////////////////////////////////////////////
class NamedProperties
{
public:
    ~NamedProperties();    

    template<typename T>
    void SetValue(std::string const& keyName, T const& value);
    template<typename T>
    T GetValue(std::string const& keyName, T const& defaultValue) const;    

    //specialized for char const
    void SetValue(std::string const& keyName, char const* value);    
    std::string GetValue(std::string const& keyName, char const* val) const;    

private:
    std::map<std::string, TypedPropertyBase*> m_keyValuePairs;

    TypedPropertyBase* FindInMap(std::string const& key) const;
};


//////////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////////
template<typename T>
bool TypedPropertyBase::Is() const
{
    return GetUniqueID() == TypedProperty<T>::StaticUniqueID();
}

//////////////////////////////////////////////////////////////////////////
template<typename VALUE_TYPE>
void const* const TypedProperty<VALUE_TYPE>::StaticUniqueID()
{
    static int s_local = 0;
    return &s_local;
}

//////////////////////////////////////////////////////////////////////////
template<typename T>
void NamedProperties::SetValue(std::string const& keyName, T const& value)
{
    TypedPropertyBase* base = FindInMap(keyName);
    if (base == nullptr) {
        TypedProperty<T>* prop = new TypedProperty<T>();
        prop->m_value = value;
        m_keyValuePairs[keyName] = prop;
    }
    else {
        if (base->Is<T>()) {
            TypedProperty<T>* prop = (TypedProperty<T>*)base;
            prop->m_value = value;
        }
        else {
            delete base;

            TypedProperty<T>* prop = new TypedProperty<T>();
            prop->m_value = value;
            m_keyValuePairs[keyName] = prop;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
template<typename T>
T NamedProperties::GetValue(std::string const& keyName, T const& defaultValue) const
{
    TypedPropertyBase* base = FindInMap(keyName);
    if (nullptr != base) {
        if (base->Is<T>()) {
            TypedProperty<T>* prop = (TypedProperty<T>*)base;
            return prop->m_value;
        }
        else {
            std::string strValue = base->GetAsString();
            return StringConvert(strValue.c_str(), defaultValue);
        }
    }
    else {
        return defaultValue;
    }

}
