#pragma once

#include "Engine/Core/StringUtils.hpp"

class Tags
{
public:
	Tags() = default;

	void ClearAllTags();
	bool HasTag( const std::string& tagName, bool isCleanTag = false ) const; //non case sensitive
	bool HasTags( const Strings& tags ) const; //non case sensitive
	void SetTags( const Strings& rawTagList );

	std::string GetDebugText() const;

protected:
	std::vector<std::string> m_tags;

	bool IsDeleteTag( const std::string& tagName ) const;

	void AddTag( const std::string& newTag );
	void RemoveTag( const std::string& tagName );
};