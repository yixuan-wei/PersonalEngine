#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//////////////////////////////////////////////////////////////////////////
SpriteAnimDefinition::SpriteAnimDefinition( const SpriteSheet& spriteSheet, int startSpriteIndex, int endSpriteIndex, float durationSeconds, eSpriteAnimPlaybackType playbackType /*= eSpriteAnimPlaybackType::LOOP */ )
	:m_spriteSheet(spriteSheet)
	,m_durationSeconds(durationSeconds)
	,m_playbackType(playbackType)
{
	if( startSpriteIndex == endSpriteIndex )
	{
		m_spriteIndexes.push_back( startSpriteIndex );
	}
	else if( startSpriteIndex < endSpriteIndex )
	{
		for( int spriteIdx = startSpriteIndex; spriteIdx <= endSpriteIndex; ++spriteIdx )
		{
			m_spriteIndexes.push_back( spriteIdx );
		}
	}
	else //startSpriteIndex > endSpriteIndex
	{
		for( int spriteIdx = startSpriteIndex; spriteIdx >= endSpriteIndex; --spriteIdx )
		{
			m_spriteIndexes.push_back( spriteIdx );
		}
	}

	GUARANTEE_OR_DIE(!m_spriteIndexes.empty(), Stringf( "sprite indexes for sprite anim definition could not be null" ) );
}

//////////////////////////////////////////////////////////////////////////
SpriteAnimDefinition::SpriteAnimDefinition( const SpriteSheet& sheet, const std::vector<int>& spriteIndexes, float durationSeconds, eSpriteAnimPlaybackType playbackType /*= eSpriteAnimPlaybackType::LOOP */ )
	:m_spriteSheet(sheet)
	,m_durationSeconds(durationSeconds)
	,m_playbackType(playbackType)
{
	GUARANTEE_OR_DIE( !spriteIndexes.empty(), Stringf( "sprite indexes for sprite anim definition could not be null" ) );

	m_spriteIndexes = spriteIndexes;
}

//////////////////////////////////////////////////////////////////////////
const Texture& SpriteAnimDefinition::GetTexture() const
{
	return m_spriteSheet.GetTexture();
}

//////////////////////////////////////////////////////////////////////////
const SpriteDefinition& SpriteAnimDefinition::GetSpriteDefAtTime( float seconds ) const
{
	int frameNumbers = (int)m_spriteIndexes.size();
	float unitFrameTime = m_durationSeconds / (float)frameNumbers;
	int index = 0;

	if( frameNumbers==1)
	{
		index = 0;
	}
	else if( m_playbackType == eSpriteAnimPlaybackType::ONCE )
	{
		float clampedSeconds = Clamp( seconds, 0.f, m_durationSeconds );
		float passedFrames = Clamp(clampedSeconds / unitFrameTime, 0.f, (float)(frameNumbers-1));
		index = RoundDownToInt( passedFrames );
	}
	else if( m_playbackType == eSpriteAnimPlaybackType::LOOP )
	{
		int passedFramesNum = RoundDownToInt( seconds / unitFrameTime );
		index = passedFramesNum % frameNumbers;		
	}
	else if( m_playbackType == eSpriteAnimPlaybackType::PINGPONG )
	{
		int passedFramesNum = RoundDownToInt( seconds / unitFrameTime );
		int posInCircle = passedFramesNum % (2 * (frameNumbers - 1));
		if( posInCircle < frameNumbers - 1 )
		{
			index = posInCircle;
		}
		else
		{
			index = 2 * (frameNumbers - 1) - posInCircle;
		}
	}
	else
	{
		ERROR_AND_DIE( Stringf( "SpriteAnimDefinition has wrong Playback type" ) );
	}

	return m_spriteSheet.GetSpriteDefinition( m_spriteIndexes[index]);
}
