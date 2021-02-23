#pragma once

#include <vector>
#include "Engine/Renderer/SpriteDefinition.hpp"

class Texture;
class SpriteDefinition;
struct IntVec2;
struct Vec2;

class SpriteSheet
{
public:
	explicit SpriteSheet( const Texture& texture, const IntVec2& simpleGridLayout );
	~SpriteSheet();
	
	const Texture&          GetTexture() const { return m_texture; }
	int                     GetNumSprites() const { return (int)m_spriteDefs.size(); }
	const SpriteDefinition& GetSpriteDefinition( int spriteIndex ) const;
	void                    GetSpriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex ) const;

protected:
	const Texture& m_texture;
	std::vector<SpriteDefinition> m_spriteDefs;
};