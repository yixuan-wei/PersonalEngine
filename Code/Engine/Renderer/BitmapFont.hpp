#pragma once

#include <string>
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Texture;
struct Vertex_PCU;
struct AABB2;

class BitmapFont
{
	friend class RenderContext; // only render context can create bitmap font

private:
	BitmapFont( const char* fontPath, const Texture* fontTexture );

public:
	std::string    GetFontPath()const { return m_fontPath; }
	const Texture* GetTexture() const;
	Vec2           GetDimensionsForText2D( float cellHeight, const std::string& text, float cellAspect = 1.f, float lineSpaceFraction = 0.05f, float kerningFraction=.0f ) const;

	int AddVertsForText2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& textMins, float cellHeight, const std::string& text, 
		const Rgba8& tint = Rgba8::WHITE, float cellAspect = 1.f, float lineSpaceFraction = 0.05f, float kerningFraction=0.f ) const;//return line numbers consumed
	int AddVertsForTextInBox2D( std::vector<Vertex_PCU>& vertexArray, const AABB2& box, float cellHeight,
		const std::string& text, const Rgba8& tint = Rgba8::WHITE, float cellAspect = 1.f,
		const Vec2& alignment = ALIGN_CENTERED, float lineSpaceFraction = 0.05f, float kerningFraction=0.f ) const; //return line numbers consumed
	int AddVertsForText3D(std::vector<Vertex_PCU>& verts, Vec3 const& textMins, Vec3 const& up, Vec3 const& right,
		float cellHeight, std::string const& text, Rgba8 const& tint=Rgba8::WHITE, float cellAspect=1.f, float lineSpaceFraction=.05f,
		float kerning=0.f) const;	//return line number consumed

protected:
	std::string m_fontPath;
	SpriteSheet m_glyphSpriteSheet;

	float GetGlyphAspect( int glyphUnicode ) const;
};