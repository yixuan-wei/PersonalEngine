#pragma once

struct Vec2;
struct Vec3;

struct Vec4
{
public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

public:
	Vec4() = default;
	~Vec4() = default;
	Vec4( const Vec4& copyFrom );
	explicit Vec4( float initialX, float initialY, float initialZ, float initialW );
	explicit Vec4(Vec3 const& copyFrom, float initialW = 0.f);

	//Accessors
	float GetLengthSquared() const;
	Vec3  GetXYZ() const;
	Vec2  GetXY() const;

	//operators(const)
	bool operator==( const Vec4& compare ) const;
	bool operator!=( const Vec4& compare ) const;

	const Vec4 operator+( const Vec4& vecToAdd ) const;
	const Vec4 operator-( const Vec4& vecToSubstract ) const;
	const Vec4 operator-() const;
	const Vec4 operator*( float uniformScale ) const;
	const Vec4 operator*( const Vec4& vecToMultiply ) const;
	const Vec4 operator/( float inverseScale ) const;

	//operator(self-mutating)
	void operator+=( const Vec4& vecToAdd );
	void operator-=( const Vec4& vecToSubstract );
	void operator*=( const float uniformScale );
	void operator/=( const float uniformDivisor );
	void operator=( const Vec4& copyFrom );

	//standalone * for float and vec4
	friend const Vec4 operator*( float uniformScale, const Vec4& vecToScale );
};