#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/MeshUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"

//////////////////////////////////////////////////////////////////////////
BitmapFont::BitmapFont( const char* fontPath, const Texture* fontTexture )
	:m_glyphSpriteSheet(SpriteSheet(*fontTexture,IntVec2(16,16)))
	,m_fontPath(fontPath)
{
	g_theConsole->PrintString( Rgba8::WHITE, Stringf( "Bitmap font initialized" ) );
}

//////////////////////////////////////////////////////////////////////////
const Texture* BitmapFont::GetTexture() const
{
	return &m_glyphSpriteSheet.GetTexture();
}

//////////////////////////////////////////////////////////////////////////
Vec2 BitmapFont::GetDimensionsForText2D( float cellHeight, const std::string& text, float cellAspect /*= 1.f */, float lineSpaceFraction, float kerningFraction ) const
{
	float width = 0.f;
	float height = cellHeight;
	float tempWidth = kerningFraction*cellHeight*cellAspect;
	float unitWidth = cellHeight * cellAspect * (1.f-kerningFraction);

	for( int tID = 0; tID < (int)text.length(); tID++ )
	{
		if( text[tID] == '\n' )
		{
			height += cellHeight + lineSpaceFraction*cellHeight;
			if( tempWidth > width )
			{
				width = tempWidth;
			}
			tempWidth = 0.f;
		}
		else
		{
			tempWidth += unitWidth;
		}
	}

	if( tempWidth > width )
	{
		width = tempWidth;
	}

	return Vec2( width, height );
}

//////////////////////////////////////////////////////////////////////////
int BitmapFont::AddVertsForText2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& textMins, float cellHeight, 
	const std::string& text, const Rgba8& tint /*= Rgba8::WHITE*/, float cellAspect /*= 1.f */, float lineSpaceFraction,
	float kerningFraction) const
{
	Vec2 charMins = textMins;
	Vec2 unitCell = Vec2( cellHeight * cellAspect, cellHeight );
	float cellWidth = cellHeight * cellAspect*(1.f-kerningFraction);
	int lineNumbers = 1;

	for( int chrID = 0; chrID < (int)text.length(); chrID++ )
	{
		int glyph = static_cast<int>(text[chrID]);
		Vec2 uvAtMins, uvAtMaxs;
		m_glyphSpriteSheet.GetSpriteUVs( uvAtMins, uvAtMaxs, glyph );

		if( glyph == (int)'\n' )
		{
			lineNumbers++;
			charMins = textMins;
			charMins.y -= cellHeight + lineSpaceFraction*cellHeight;
			continue;
		}

		AppendVertsForAABB2D( vertexArray, AABB2( charMins, charMins + unitCell ), uvAtMins, uvAtMaxs, tint );
		charMins.x += cellWidth;
	}

	return lineNumbers;
}

//////////////////////////////////////////////////////////////////////////
int BitmapFont::AddVertsForTextInBox2D( std::vector<Vertex_PCU>& vertexArray, const AABB2& box, float cellHeight,
	const std::string& text, const Rgba8& tint /*= Rgba8::WHITE*/, float cellAspect /*= 1.f*/, const Vec2& alignment /*= ALIGN_CENTERED */,
	float lineSpaceFraction, float kerningFraction) const
{
	Vec2 textDim = GetDimensionsForText2D( cellHeight, text, cellAspect, lineSpaceFraction, kerningFraction );
	if( textDim.x == 0.f || textDim.y == 0.f )
		return 0;

	Vec2 bottomLeftMaxs = box.maxs - textDim;
	if( bottomLeftMaxs.x < box.mins.x )
		bottomLeftMaxs.x = box.mins.x;
	if( bottomLeftMaxs.y < box.mins.y )
		bottomLeftMaxs.y = box.mins.y;

	Vec2 bottomLeftPos;
	bottomLeftPos.x = RangeMapFloat( 0.f, 1.f, box.mins.x, bottomLeftMaxs.x, alignment.x );
	bottomLeftPos.y = RangeMapFloat( 0.f, 1.f, box.mins.y, bottomLeftMaxs.y, alignment.y );
	Vec2 startPos = bottomLeftPos;

	float unitWidth = cellHeight * cellAspect * (1.f-kerningFraction);
	int textLength = (int)text.size();
	int lineEndPos = textLength - 1;
	int lineNumbers = 0;
	//from back to minimum find the '\n' then add vertex
	for( int charID = lineEndPos; charID >-1; charID-- )
	{
		if( text[charID] == '\n' )
		{
			//print text from charID+1 to lineEndPos
			int lineChrNum = lineEndPos - charID;
			float widthVacant = textDim.x - (float)lineChrNum * unitWidth - kerningFraction*cellHeight*cellAspect;
			if( widthVacant > 0.f )
			{
				bottomLeftPos.x += widthVacant * alignment.x;
			}

			std::string thisLine (text, (size_t)charID + 1, lineChrNum );
			AddVertsForText2D( vertexArray, bottomLeftPos, cellHeight, thisLine, tint, cellAspect, lineSpaceFraction, kerningFraction );

			lineEndPos = charID - 1;
			bottomLeftPos.x = startPos.x;
			bottomLeftPos.y += cellHeight + cellHeight*lineSpaceFraction;
			lineNumbers += 1;
		}
	}
	//last line
	float widthVacant = textDim.x - (float)(lineEndPos+1) * unitWidth - kerningFraction * cellHeight * cellAspect;
	if( widthVacant > 0.f )
	{
		bottomLeftPos.x += widthVacant * alignment.x;
	}
	std::string thisLine (text, 0, (size_t)lineEndPos+1 );
	AddVertsForText2D( vertexArray, bottomLeftPos, cellHeight, thisLine, tint, cellAspect, lineSpaceFraction, kerningFraction);
	lineNumbers += 1;

	return lineNumbers;
}

//////////////////////////////////////////////////////////////////////////
int BitmapFont::AddVertsForText3D(std::vector<Vertex_PCU>& verts, Vec3 const& textMins, 
	Vec3 const& up, Vec3 const& right, float cellHeight, std::string const& text, 
	Rgba8 const& tint/*=Rgba8::WHITE*/, float cellAspect/*=1.f*/, float lineSpaceFraction/*=.05f*/, 
	float kerningFraction/*=0.f*/) const
{
    Vec3 charMins = textMins;
    Vec3 unitCell = cellHeight * cellAspect *right+ cellHeight*up;
    float cellWidth = cellHeight * cellAspect * (1.f - kerningFraction);
	Vec3 unitRight = cellHeight*cellAspect*right;
	Vec3 unitUp = cellHeight*up;
    int lineNumbers = 1;

    for (int chrID = 0; chrID < (int)text.length(); chrID++)
    {
        int glyph = static_cast<int>(text[chrID]);
        Vec2 uvAtMins, uvAtMaxs;
        m_glyphSpriteSheet.GetSpriteUVs(uvAtMins, uvAtMaxs, glyph);

        if (glyph == (int)'\n')
        {
            lineNumbers++;
            charMins -= (1.f+lineSpaceFraction) * cellHeight*up;
            continue;
        }

        AppendIndexedVertexesForQuaterPolygon2D(verts, charMins, charMins+unitRight, charMins+unitCell,
			charMins+unitUp, tint, uvAtMins, uvAtMaxs);
        charMins += cellWidth*right;
    }

    return lineNumbers;
}

//////////////////////////////////////////////////////////////////////////
float BitmapFont::GetGlyphAspect( int glyphUnicode ) const
{
	if( m_glyphSpriteSheet.GetNumSprites() < glyphUnicode+1 )
		return 0;

	SpriteDefinition oneDef = m_glyphSpriteSheet.GetSpriteDefinition( glyphUnicode );
	return oneDef.GetAspect();
}
