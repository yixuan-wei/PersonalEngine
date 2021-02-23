#pragma once

struct Vec2;
struct Vec3;

class RandomNumberGenerator
{
public:
	int   RollRandomIntLessThan( int maxNotInclusive );
	int   RollRandomIntInRange( int minInclusive, int maxInclusive );
	float RollRandomFloatLessThan( float maxNotInclusive );
	float RollRandomFloatZeroToOneInclusive();
	float RollRandomFloatZeroToAlmostOne();
	float RollRandomFloatInRange( float minInclusive, float maxInclusive );
	bool  RollPercentChance( float probabilityOfReturningTrue );
	Vec2  RollRandomDirection2D();
	Vec3  RollRandomDirection3D();

	void  Reset( unsigned int seed = 0 );

	unsigned int GetSeed() const { return m_seed; }

private:
	unsigned int  m_seed = 0;	
	unsigned int  m_position = 0;
};
