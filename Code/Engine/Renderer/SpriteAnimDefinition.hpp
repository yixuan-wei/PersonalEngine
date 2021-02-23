#pragma once

#include "Engine/Renderer/SpriteSheet.hpp"

enum eSpriteAnimPlaybackType
{
	ONCE,		// for 5-frame animatinon, plays 0,1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4...
	LOOP,		// for 5-frame animation, plays 0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0...
	PINGPONG,	// for 5-frame animation, plays 0,1,2,3,4,3,2,1,0,1,2,3,4,3,2,1,0,1,2,3,4,3,2,1,0,1...
};

class SpriteAnimDefinition
{
public:
	SpriteAnimDefinition( const SpriteSheet& spriteSheet, int startSpriteIndex, int endSpriteIndex,
		float durationSeconds, eSpriteAnimPlaybackType playbackType = eSpriteAnimPlaybackType::LOOP );
	SpriteAnimDefinition( const SpriteSheet& sheet, const std::vector<int>& spriteIndexes,
		float durationSeconds, eSpriteAnimPlaybackType playbackType = eSpriteAnimPlaybackType::LOOP );

	const Texture&          GetTexture()const;
	const SpriteDefinition& GetSpriteDefAtTime( float seconds )const;

private:
	std::vector<int>       m_spriteIndexes;
	const SpriteSheet&     m_spriteSheet;
	float                  m_durationSeconds = 1.f;
	eSpriteAnimPlaybackType m_playbackType = eSpriteAnimPlaybackType::LOOP;
};