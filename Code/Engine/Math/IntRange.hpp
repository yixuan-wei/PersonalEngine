#pragma once

#include <string>

class RandomNumberGenerator;

struct IntRange
{
public:
	int minimum = 0;
	int maximum = 0;

public:
	IntRange() = default;
	explicit IntRange( int minAndMax );
	explicit IntRange( int initStart, int initEnd );
	explicit IntRange( const char* asText );
	~IntRange() = default;

	//Accessors
	bool IsInRange( int value )const;
	bool DoesOverlap( const IntRange& otherRange ) const;
	std::string GetAsString() const;
	int GetRandomInRange( RandomNumberGenerator& rng ) const;

	//Mutators
	void Set( int newMinimum, int newMaximum );
	bool SetFromText( const char* text );
};