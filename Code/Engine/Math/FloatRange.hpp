#pragma once

#include <string>

class RandomNumberGenerator;

struct FloatRange
{
public:
	float minimum = 0.f;
	float maximum = 0.f;

public:
	FloatRange() = default;
	explicit FloatRange( float minAndMax );
	explicit FloatRange( float initStart, float initEnd );
	explicit FloatRange( const char* asText );
	~FloatRange() = default;

	//Accessors
	bool        IsInRange( float value ) const;
	bool        DoesOverlap( const FloatRange& otherRange )const;
	std::string GetAsString() const;
	float       GetRandomInRange( RandomNumberGenerator& rng ) const;

	bool operator!=( const FloatRange& compare ) const;

	//Mutators
	void Set( float newMinimum, float newMaximum );
	bool SetFromText( const char* text );	
};