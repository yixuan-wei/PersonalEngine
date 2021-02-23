#include "Engine/Core/Tags.hpp"

//////////////////////////////////////////////////////////////////////////
void Tags::ClearAllTags()
{
	m_tags.clear();
}

//////////////////////////////////////////////////////////////////////////
bool Tags::HasTag( const std::string& tagName, bool isCleanTag ) const
{
	std::string target = tagName;
	if( !isCleanTag )
	{
		target = Trim( target );
		target = GetLowerCases( target );
	}

	for( std::string tag : m_tags )
	{
		if( tag == target )
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool Tags::HasTags( const Strings& tags ) const
{
	for( std::string tag : tags )
	{
		if( !HasTag( tag ) )
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
void Tags::SetTags( const Strings& rawTagList )
{
	for( std::string rawTag : rawTagList )
	{
		std::string newTag = Trim( rawTag );
		newTag = GetLowerCases( newTag );

		if( IsDeleteTag( newTag ) )
		{
			RemoveTag( newTag.substr( 1 ) );
		}
		else
		{
			AddTag( newTag );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
std::string Tags::GetDebugText() const
{
	std::string result;
	for( std::string tag : m_tags )
	{
		result += tag;
		result += ", ";
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////
void Tags::AddTag( const std::string& newTag )
{
	if( HasTag( newTag, true ) )
		return;

	m_tags.push_back( newTag );
}

//////////////////////////////////////////////////////////////////////////
void Tags::RemoveTag( const std::string& tagName )
{
	for( std::vector<std::string>::iterator tagIter = m_tags.begin(); tagIter < m_tags.end(); tagIter++ )
	{
		if( *tagIter == tagName )
		{
			m_tags.erase( tagIter );
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
bool Tags::IsDeleteTag( const std::string& tagName ) const
{
	if( !tagName.empty() && tagName[0] == '!' )
		return true;

	return false;
}
