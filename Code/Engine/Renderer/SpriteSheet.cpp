#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Math/Vec2.hpp"

//////////////////////////////////////////////////////////////////////////
SpriteSheet::SpriteSheet( const Texture& texture, const IntVec2& simpleGridLayout )
	:m_texture(texture)
{
	const IntVec2 textureSize = texture.GetTextureSize();
	float uvUnitX = 1.0f / (float)simpleGridLayout.x;
	float uvUnitY = 1.0f / (float)simpleGridLayout.y;

	int totalSprites = simpleGridLayout.x * simpleGridLayout.y;
	for( int spriteID = 0; spriteID < totalSprites; spriteID++ )
	{
		int row = spriteID / simpleGridLayout.x;
		int col = spriteID - simpleGridLayout.x * row;
		Vec2 uvAtMins( (float)col * uvUnitX, (float)(simpleGridLayout.y-1-row) * uvUnitY );
		Vec2 uvAtMaxs( uvAtMins.x + uvUnitX, uvAtMins.y + uvUnitY );
		SpriteDefinition* newDef = new SpriteDefinition( *this, spriteID, uvAtMins, uvAtMaxs );
		m_spriteDefs.push_back( *newDef );
	}
}

//////////////////////////////////////////////////////////////////////////
SpriteSheet::~SpriteSheet()
{
	m_spriteDefs.clear();
}

//////////////////////////////////////////////////////////////////////////
const SpriteDefinition& SpriteSheet::GetSpriteDefinition( int spriteIndex ) const
{
	if( spriteIndex < 0 || spriteIndex >= (int)m_spriteDefs.size() )
		return m_spriteDefs[0];
	else
		return m_spriteDefs[spriteIndex];
}

//////////////////////////////////////////////////////////////////////////
void SpriteSheet::GetSpriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex ) const
{
	if(spriteIndex>=0 && spriteIndex<(int)m_spriteDefs.size()){
		m_spriteDefs[spriteIndex].GetUVs( out_uvAtMins, out_uvAtMaxs );
	}
}
