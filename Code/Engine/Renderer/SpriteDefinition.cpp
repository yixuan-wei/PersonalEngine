#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"

//////////////////////////////////////////////////////////////////////////
SpriteDefinition::SpriteDefinition( const SpriteSheet& spriteSheet, int spriteIndex, const Vec2& uvAtMins, const Vec2& uvAtMaxs )
	:m_spriteSheet(spriteSheet)
	,m_spriteIndex(spriteIndex)
	,m_uvAtMins(uvAtMins)
	,m_uvAtMaxs(uvAtMaxs)
{
}

//////////////////////////////////////////////////////////////////////////
void SpriteDefinition::GetUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs ) const
{
	out_uvAtMins = m_uvAtMins;
	out_uvAtMaxs = m_uvAtMaxs;
}

//////////////////////////////////////////////////////////////////////////
const Texture& SpriteDefinition::GetTexture() const
{
	return m_spriteSheet.GetTexture();
}

//////////////////////////////////////////////////////////////////////////
float SpriteDefinition::GetAspect() const
{
	Vec2 relativeSize = m_uvAtMaxs - m_uvAtMins;
	const Texture& texture = GetTexture();
	IntVec2 textureSize = texture.GetTextureSize();
	return (relativeSize.x * (float)textureSize.x) / (relativeSize.y * (float)textureSize.y);
}
