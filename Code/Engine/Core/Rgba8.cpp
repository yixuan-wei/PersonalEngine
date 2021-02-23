#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <string>

const Rgba8 Rgba8::WHITE = Rgba8( 255, 255, 255, 255 );
const Rgba8 Rgba8::RED = Rgba8( 255, 0, 0, 255 );
const Rgba8 Rgba8::YELLOW = Rgba8( 255, 255, 0, 255 );
const Rgba8 Rgba8::GREEN = Rgba8( 0, 255, 0, 255 );
const Rgba8 Rgba8::BLUE = Rgba8( 0, 0, 255, 255 );
const Rgba8 Rgba8::MAGENTA = Rgba8(255,0,255,255);
const Rgba8 Rgba8::BLACK = Rgba8( 0, 0, 0, 255 );

//////////////////////////////////////////////////////////////////////////
Rgba8::Rgba8( unsigned char inputR, unsigned char inputG, unsigned char inputB, unsigned char inputA /*= 255*/ )
	: r(inputR)
	, g(inputG)
	, b(inputB)
	, a(inputA)
{
}

//////////////////////////////////////////////////////////////////////////
Rgba8::Rgba8(Vec4 const& floats)
{
	r = (unsigned char)(floats.x * 255.f);
	g = (unsigned char)(floats.y * 255.f);
	b = (unsigned char)(floats.z * 255.f);
	a = (unsigned char)(floats.w * 255.f);
}

//////////////////////////////////////////////////////////////////////////
void Rgba8::SetFromText( const char* text )
{
	if (text == nullptr) {
		return;
	}

	Strings subStrings = SplitStringOnDelimiter( Stringf( text ), ',' );
	size_t valueNum = subStrings.size();
	if (valueNum < 3 || valueNum > 4) { 
		g_theConsole->PrintError(Stringf("Rgba8 can't construct from improper string \"%s\"", text) ); 
		return;
	}

	r = (unsigned char)std::atoi( subStrings[0].c_str() );
	g = (unsigned char)std::atoi( subStrings[1].c_str() );
	b = (unsigned char)std::atoi( subStrings[2].c_str() );
	if( valueNum == 4 )
	{
		a = (unsigned char)std::atoi( subStrings[3].c_str() );
	}
	else
	{ 
		a = 255; 
	}
}

//////////////////////////////////////////////////////////////////////////
Rgba8 Rgba8::GetTintedColor(Rgba8 const& tint) const
{
	Vec4 self = ToFloats();
	Vec4 tintF = tint.ToFloats();
	Vec4 newSelf = self * tintF;

	return Rgba8(newSelf);
}

//////////////////////////////////////////////////////////////////////////
Vec3 Rgba8::ToHSL() const
{
	Vec4 rgba = ToFloats();
	float maxValue = MaxFloat(rgba.x, rgba.y, rgba.z);
	float minValue = MinFloat(rgba.x, rgba.y, rgba.z);
	float delta = maxValue - minValue;

	Vec3 hsl;
	//hue
	if (delta == 0.f){
		hsl = Vec3::ZERO;
	}
	else if (maxValue==rgba.x) {
		hsl.x = 60.f * (rgba.y - rgba.z) / delta;
	}
	else if (maxValue == rgba.y) {
		hsl.x = 60.f * ((rgba.z - rgba.x) / delta + 2.f);
	}
	else {//B is max
		hsl.x = 60.f * ((rgba.x - rgba.y) / delta + 4.f);
	}
	//lighting
	hsl.z = (maxValue + minValue) * .5f;
	//saturation
	hsl.y = delta == 0.f ? 0.f : (delta / (1.f - AbsFloat(2.f * hsl.z - 1.f)));

	return hsl;
}

//////////////////////////////////////////////////////////////////////////
Vec4 Rgba8::ToFloats() const
{
	float division = 1.f / 255.f;
	return Vec4((float)r * division, (float)g * division, (float)b * division, (float)a * division);
}

//////////////////////////////////////////////////////////////////////////
bool Rgba8::operator!=(const Rgba8& compareWith)
{
	if (r != compareWith.r || g != compareWith.g || b != compareWith.b || a != compareWith.a) {
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
void Rgba8::operator=( const Rgba8& copyFrom )
{
	r = copyFrom.r;
	g = copyFrom.g;
	b = copyFrom.b;
	a = copyFrom.a;
}

//////////////////////////////////////////////////////////////////////////
bool Rgba8::operator==( const Rgba8& compareWith )
{
	if( r == compareWith.r && g == compareWith.g && b == compareWith.b && a==compareWith.a )
		return true;
	else 
		return false;
}

//////////////////////////////////////////////////////////////////////////
Rgba8 Lerp(Rgba8 const& colorA, Rgba8 const& colorB, float alpha /*= .5f*/)
{
	float invAlpha = 1.f - alpha;
	float r = alpha * (float)colorA.r + invAlpha * (float)colorB.r;
	float g = alpha * (float)colorA.g + invAlpha * (float)colorB.g;
	float b = alpha * (float)colorA.b + invAlpha * (float)colorB.b;
	float a = alpha * (float)colorA.a + invAlpha * (float)colorB.a;
	return Rgba8((unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a);
}

//////////////////////////////////////////////////////////////////////////
Rgba8 LerpAsHSL(Rgba8 const& colorA, Rgba8 const& colorB, float alpha /*= .5f*/)
{
	float invMax = 1.f / 255.f;
	Vec3 hslA = colorA.ToHSL();
	Vec3 hslB = colorB.ToHSL();
	Vec4 hslaA(hslA, (float)colorA.a*invMax);
	Vec4 hslaB(hslB, (float)colorB.a*invMax);

	Vec4 hsla = alpha * hslaA + (1.f - alpha) * hslaB;
	Vec3 rgb = GetRGBFromHSL(Vec3(hsla.x, hsla.y, hsla.z));
	Vec4 rgba(rgb, hsla.w);
	return Rgba8(rgba);
}

//////////////////////////////////////////////////////////////////////////
Vec3 GetRGBFromHSL(Vec3 const& raw)
{
	Vec3 hsl = raw;
	hsl.x = Clamp(raw.x, 0.f, 360.f);
	hsl.y = ClampZeroToOne(raw.y);
	hsl.z = ClampZeroToOne(raw.z);

	float chroma = (1.f - AbsFloat(2.f * hsl.z - 1.f)) * hsl.y;
	float _h = hsl.x / 60.f;
	float division = _h * .5f;
	float _hMod2 = _h - (float)((int)division) * 2.f;
	float x = chroma * (1.f - AbsFloat(_hMod2 - 1));

	Vec3 rgb1;
	int _hInt = int(_h);
	switch (_hInt)
	{
	case 0:	rgb1 = Vec3(chroma, x, 0.f); break;
	case 1:	rgb1 = Vec3(x, chroma, 0.f); break;
	case 2:	rgb1 = Vec3(0.f, chroma, x); break;
	case 3: rgb1 = Vec3(0.f, x, chroma); break;
	case 4: rgb1 = Vec3(x, 0.f, chroma); break;
	case 5: rgb1 = Vec3(chroma, 0.f, x); break;
	}

	float m = hsl.z - chroma * .5f;
	return rgb1 + Vec3(m);
}

