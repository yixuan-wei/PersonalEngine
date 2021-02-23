#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/RawNoise.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
int RandomNumberGenerator::RollRandomIntLessThan( int maxNotInclusive )
{
	unsigned int randomBits = Get1dNoiseUint( m_position++, m_seed );
	return randomBits % maxNotInclusive;
}

//////////////////////////////////////////////////////////////////////////
int RandomNumberGenerator::RollRandomIntInRange( int minInclusive, int maxInclusive )
{
	return RollRandomIntLessThan(maxInclusive - minInclusive + 1) + minInclusive;
}

//////////////////////////////////////////////////////////////////////////
float RandomNumberGenerator::RollRandomFloatLessThan( float maxNotInclusive )
{
	return maxNotInclusive * RollRandomFloatZeroToAlmostOne();
}

//////////////////////////////////////////////////////////////////////////
float RandomNumberGenerator::RollRandomFloatZeroToOneInclusive()
{
	return Get1dNoiseZeroToOne(m_position++,m_seed);
}

//////////////////////////////////////////////////////////////////////////
float RandomNumberGenerator::RollRandomFloatZeroToAlmostOne()
{
	constexpr double ONE_OVER_MAX_UINT_ONE = (1.0 / ((double)0xFFFFFFFF + 1.0));
	return (float)(ONE_OVER_MAX_UINT_ONE * (double)Get1dNoiseUint( m_position++, m_seed ));
}

//////////////////////////////////////////////////////////////////////////
float RandomNumberGenerator::RollRandomFloatInRange( float minInclusive, float maxInclusive )
{
	return RollRandomFloatZeroToOneInclusive()*(maxInclusive - minInclusive) + minInclusive;
}

//////////////////////////////////////////////////////////////////////////
bool RandomNumberGenerator::RollPercentChance( float probabilityOfReturningTrue )
{
	float percent = RollRandomFloatZeroToOneInclusive();
	if( probabilityOfReturningTrue >= percent )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
Vec2 RandomNumberGenerator::RollRandomDirection2D()
{
	float degrees = RollRandomFloatLessThan( 360.f );
	return Vec2::MakeFromPolarDegrees(degrees);
}

//////////////////////////////////////////////////////////////////////////
Vec3 RandomNumberGenerator::RollRandomDirection3D()
{
	float theta = RollRandomFloatLessThan(360.f);
	float phita = RollRandomFloatLessThan(180.f) - 90.f;
	Vec2 thetaValue = Vec2::MakeFromPolarDegrees(theta);
	Vec2 phitaValue = Vec2::MakeFromPolarDegrees(phita);
	return Vec3(thetaValue.x * phitaValue.x, phitaValue.y, thetaValue.y * phitaValue.x);
}

//////////////////////////////////////////////////////////////////////////
void RandomNumberGenerator::Reset( unsigned int seed /*= 0 */ )
{
	m_seed = seed;
	m_position = 0;
}

