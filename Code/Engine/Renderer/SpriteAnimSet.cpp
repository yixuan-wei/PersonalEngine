#include "Engine/Renderer/SpriteAnimSet.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/IntVec2.hpp"

//////////////////////////////////////////////////////////////////////////
SpriteAnimSet::SpriteAnimSet( RenderContext& renderer, const XmlElement& spriteAnimSetElement )
{
	std::string sheetPath = ParseXmlAttribute( spriteAnimSetElement, "spriteSheet", "" );	
	IntVec2 sheetLayout = ParseXmlAttribute( spriteAnimSetElement, "spriteLayout", IntVec2( 1, 1 ) );
	float fps = ParseXmlAttribute( spriteAnimSetElement, "fps", 30.f );
	GUARANTEE_OR_DIE( !sheetPath.empty(), Stringf( "SpriteAnimSet has wrong name %s for spriteSheet", sheetPath.c_str() ) );

	const Texture* sheetTexture = renderer.CreateOrGetTextureFromFile( sheetPath.c_str() );
	SpriteSheet* sheet = new SpriteSheet( *sheetTexture, sheetLayout );

	//go over sprite animation definition
	const XmlElement* child = spriteAnimSetElement.FirstChildElement();
	Ints defaultSpriteIndexes;
	defaultSpriteIndexes.push_back( 0 );
	while( child != nullptr )
	{
		GUARANTEE_OR_DIE( child->Name() == Stringf( "SpriteAnim" ),
			Stringf( "child %s in SpriteAnimSet xml is illegal", child->Name() ) );

		std::string animName = ParseXmlAttribute( *child, "name", "" );
		Ints spriteIndexes = ParseXmlAttribute( *child, "spriteIndexes", defaultSpriteIndexes );
		float animFps = ParseXmlAttribute( *child, "fps", fps );
		GUARANTEE_OR_DIE( !animName.empty(), Stringf( "One Sprite Anim Name is wrong" ) );

		float duration = (float)spriteIndexes.size() / animFps;
		SpriteAnimDefinition* newAnim = new SpriteAnimDefinition( *sheet, spriteIndexes, duration );
		m_animations[animName] = newAnim;

		child = child->NextSiblingElement();
	}
	GUARANTEE_OR_DIE( !m_animations.empty(), Stringf( "spriteAnimSet has no anim definition" ) );
}

//////////////////////////////////////////////////////////////////////////
void SpriteAnimSet::GetUVsForNameAtTime( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, const std::string& name, float seconds ) const
{
	std::map<std::string, SpriteAnimDefinition*>::const_iterator animIter = m_animations.find( name );
	GUARANTEE_OR_DIE( animIter != m_animations.end(), Stringf( "this SpriteAnimSet has no animation named %s", name.c_str() ) );

	const SpriteAnimDefinition* animDef = animIter->second;
	const SpriteDefinition& spriteDef = animDef->GetSpriteDefAtTime( seconds );
	spriteDef.GetUVs( out_uvAtMins, out_uvAtMaxs );
}

//////////////////////////////////////////////////////////////////////////
const Texture& SpriteAnimSet::GetTexture(const std::string& name) const
{
	std::map<std::string, SpriteAnimDefinition*>::const_iterator animIter = m_animations.find( name );
	GUARANTEE_OR_DIE( animIter != m_animations.end(), Stringf( "this SpriteAnimSet has no animation named %s", name.c_str() ) );

	const SpriteAnimDefinition* animDef = animIter->second;
	return animDef->GetTexture();
}

//////////////////////////////////////////////////////////////////////////
float SpriteAnimSet::GetAspect(const std::string& name) const
{
	std::map<std::string, SpriteAnimDefinition*>::const_iterator animIter = m_animations.find( name );
	GUARANTEE_OR_DIE( animIter != m_animations.end(), Stringf( "this SpriteAnimSet has no animation named %s", name.c_str() ) );

	const SpriteAnimDefinition* animDef = animIter->second;
	const SpriteDefinition& spriteDef = animDef->GetSpriteDefAtTime( 0.f );
	return spriteDef.GetAspect();
}
