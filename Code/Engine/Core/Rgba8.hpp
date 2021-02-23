#pragma once

struct Rgba8;
struct Vec4;
struct Vec3;

Rgba8 Lerp(Rgba8 const& colorA, Rgba8 const& colorB, float alpha = .5f);
Rgba8 LerpAsHSL(Rgba8 const& colorA, Rgba8 const& colorB, float alpha = .5f);
Vec3 GetRGBFromHSL(Vec3 const& hsl);

struct Rgba8
{
public:
	unsigned char r = 255;
	unsigned char g = 255;
	unsigned char b = 255;
	unsigned char a = 255;

	static const Rgba8 WHITE;
	static const Rgba8 RED;
	static const Rgba8 YELLOW;
	static const Rgba8 GREEN;
	static const Rgba8 BLUE;
	static const Rgba8 MAGENTA;
	static const Rgba8 BLACK;

public:
	Rgba8(){}
	explicit Rgba8(Vec4 const& floats);
	explicit Rgba8( unsigned char inputR, unsigned char inputG, unsigned char inputB, unsigned char inputA = 255);

	void SetFromText( const char* text );

	Rgba8 GetTintedColor(Rgba8 const& tint) const;
	Vec3 ToHSL() const;
	Vec4 ToFloats() const;

	void operator=( const Rgba8& copyFrom );
	bool operator!=(const Rgba8& compareWith);
	bool operator==( const Rgba8& compareWith );//compares rgba right now
};