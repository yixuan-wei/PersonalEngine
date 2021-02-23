#include "SimpleTriangleFont.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"


//------------------------------------------------------------------------------------------------
constexpr int TRITEXT_PIX_WIDE = 5;
constexpr int TRITEXT_PIX_HIGH = 9;
constexpr int TRITEXT_PIX_PER_GLYPH = TRITEXT_PIX_WIDE * TRITEXT_PIX_HIGH;
constexpr int TRITEXT_FIRST_ASCII = 32;
constexpr int TRITEXT_LAST_ASCII = 126;
constexpr int TRITEXT_NUM_ASCIIS = 1 + TRITEXT_LAST_ASCII - TRITEXT_FIRST_ASCII;
constexpr int TRITEXT_NUM_ROWS = TRITEXT_NUM_ASCIIS * TRITEXT_PIX_HIGH;
constexpr int TRITEXT_NUM_PIXELS = TRITEXT_NUM_ASCIIS * TRITEXT_PIX_PER_GLYPH;

//------------------------------------------------------------------------------------------------
// 5x7 "pixel" data for each ASCII character from 32 (space) through 126 (~ tilde)
// Note: indexes are offset by -32 from ASCII value, so space(32) is at [0] at A(65) is at [33]
const char* g_triTextFontData[ TRITEXT_NUM_ROWS ] = 
{
	//SPACE	  33 !     34 "     35 #     36 $     37 %     38 &     39 '     40 (     41 )     42 *     43 +     44 ,     45 -     46 .     47 /     48 '0'   49 '1'   50 '2'   51 '3'   52 '4'   53 '5'   54 '6'   55 '7'   56 '8'   57 '9'   58 :     59 ;     60 <     61 =     62 >     63 ?     64 @     65 A     66 B     67 C     68 D     69 E     70 F     71 G     72 H     73 I     74 J     75 K     76 L     77 M     78 N     79 O     80 P     81 Q     82 R     83 S     84 T     85 U     86 V     87 W     88 X     89 Y     90 Z     91 [     92 \     93 ]     94 ^     95 _     96 `     97 a     98 b     99 c     100 d    101 e    102 f    103 g    104 h    105 i    106 j    107 k    108 l    109 m    110 n    111 o    112 p    113 q    114 r    115 s    116 t    117 u    118 v    119 w    120 x    121 y    122 z    123 {    124 |    125 }    126 ~   
	".....", "..O..", ".O.O.", ".O.O.", "..O..", "OO..O", ".OO..", "..O..", "...O.", ".O...", "..O..", ".....", ".....", ".....", ".....", ".....", ".OOO.", "..O..", ".OOO.", ".OOO.", "O..O.", "OOOOO", ".OOO.", "OOOOO", ".OOO.", ".OOO.", ".....", ".....", "...O.", ".....", ".O...", ".OOO.", ".OOO.", ".OOO.", "OOOO.", ".OOO.", "OOOO.", "OOOOO", "OOOOO", ".OOO.", "O...O", "OOOOO", ".OOOO", "O...O", "O....", "OO.OO", "O...O", ".OOO.", "OOOO.", ".OOO.", "OOOO.", ".OOO.", "OOOOO", "O...O", "O...O", "O...O", "O...O", "O...O", "OOOOO", ".OOO.", ".....", ".OOO.", "..O..", ".....", ".OO..", ".....", "O....", ".....", "....O", ".....", "..OOO", ".....", "O....", "..O..", "...O.", "O....", ".OO..", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".O...", ".....", ".....", ".....", ".....", ".....", ".....", "..OO.", "..O..", ".OO..", ".....", 
	".....", "..O..", ".O.O.", ".O.O.", ".OOOO", "OO..O", "O..O.", "..O..", "..O..", "..O..", "O.O.O", "..O..", ".....", ".....", ".....", "....O", "O...O", ".OO..", "O...O", "O...O", "O..O.", "O....", "O....", "....O", "O...O", "O...O", ".....", ".....", "..O..", ".....", "..O..", "O...O", "O...O", "O...O", "O...O", "O...O", "O...O", "O....", "O....", "O...O", "O...O", "..O..", "...O.", "O..O.", "O....", "O.O.O", "OO..O", "O...O", "O...O", "O...O", "O...O", "O...O", "..O..", "O...O", "O...O", "O...O", "O...O", "O...O", "....O", ".O...", "O....", "...O.", ".O.O.", ".....", "..OO.", ".....", "O....", ".....", "....O", ".....", ".O...", ".....", "O....", ".....", ".....", "O....", "..O..", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".O...", ".....", ".....", ".....", ".....", ".....", ".....", ".O...", "..O..", "...O.", ".OO.O", 
	".....", "..O..", ".....", "OOOOO", "O.O..", "...O.", "O.O..", ".....", ".O...", "...O.", ".OOO.", "..O..", ".....", ".....", ".....", "...O.", "O..OO", "..O..", "...O.", "....O", "O..O.", "OOOO.", "O....", "...O.", "O...O", "O...O", "..O..", "..O..", ".O...", "OOOOO", "...O.", "...O.", "O..OO", "O...O", "O...O", "O....", "O...O", "O....", "O....", "O....", "O...O", "..O..", "...O.", "O.O..", "O....", "O.O.O", "O.O.O", "O...O", "O...O", "O...O", "O...O", "O....", "..O..", "O...O", "O...O", "O...O", ".O.O.", ".O.O.", "...O.", ".O...", ".O...", "...O.", "O...O", ".....", "...O.", ".OOO.", "OOOO.", ".OOO.", ".OOOO", ".OOO.", "OOOO.", ".OOOO", "OOOO.", ".OO..", ".OOO.", "O...O", "..O..", "OOOO.", "O.OO.", ".OOO.", "OOOO.", ".OOOO", "O.OOO", ".OOOO", "OOOOO", "O...O", "O...O", "O...O", "O...O", "O...O", "OOOOO", ".O...", "..O..", "...O.", "O..O.", 
	".....", "..O..", ".....", ".O.O.", ".OOO.", "..O..", ".O...", ".....", ".O...", "...O.", "OOOOO", "OOOOO", ".....", "OOOOO", ".....", "..O..", "O.O.O", "..O..", "..O..", ".OOO.", "OOOOO", "....O", "OOOO.", "..O..", ".OOO.", ".OOOO", ".....", ".....", "O....", ".....", "....O", "..O..", "O.O.O", "OOOOO", "OOOO.", "O....", "O...O", "OOO..", "OOO..", "O.OOO", "OOOOO", "..O..", "...O.", "OO...", "O....", "O.O.O", "O..OO", "O...O", "OOOO.", "O...O", "OOOO.", ".OOO.", "..O..", "O...O", "O...O", "O.O.O", "..O..", "..O..", "..O..", ".O...", "..O..", "...O.", ".....", ".....", ".....", "....O", "O...O", "O...O", "O...O", "O...O", ".O...", "O...O", "O...O", "..O..", "...O.", "O..O.", "..O..", "O.O.O", "OO..O", "O...O", "O...O", "O...O", "OO...", "O....", ".O...", "O...O", "O...O", "O...O", ".O.O.", "O...O", "...O.", "O....", "..O..", "....O", ".....", 
	".....", "..O..", ".....", "OOOOO", "..O.O", ".O...", "O.O.O", ".....", ".O...", "...O.", ".OOO.", "..O..", ".....", ".....", ".....", ".O...", "OO..O", "..O..", ".O...", "....O", "...O.", "....O", "O...O", "..O..", "O...O", "....O", "..O..", "..O..", ".O...", "OOOOO", "...O.", "..O..", "O.OOO", "O...O", "O...O", "O....", "O...O", "O....", "O....", "O...O", "O...O", "..O..", "...O.", "O.O..", "O....", "O...O", "O...O", "O...O", "O....", "O.O.O", "O.O..", "....O", "..O..", "O...O", "O...O", "O.O.O", ".O.O.", "..O..", ".O...", ".O...", "...O.", "...O.", ".....", ".....", ".....", ".OOOO", "O...O", "O....", "O...O", "OOOOO", ".O...", "O...O", "O...O", "..O..", "...O.", "OOO..", "..O..", "O.O.O", "O...O", "O...O", "O...O", "O...O", "O....", ".OOO.", ".O...", "O...O", "O...O", "O.O.O", "..O..", ".O.O.", "..O..", ".O...", "..O..", "...O.", ".....", 
	".....", ".....", ".....", ".O.O.", "OOOO.", "O..OO", "O..O.", ".....", ".O...", "...O.", "O.O.O", "..O..", ".....", ".....", ".....", "O....", "O...O", "..O..", "O....", "O...O", "...O.", "O...O", "O...O", "..O..", "O...O", "....O", ".....", "..O..", "..O..", ".....", "..O..", ".....", "O....", "O...O", "O...O", "O...O", "O...O", "O....", "O....", "O...O", "O...O", "..O..", "O..O.", "O..O.", "O....", "O...O", "O...O", "O...O", "O....", "O..O.", "O..O.", "O...O", "..O..", "O...O", ".O.O.", "OO.OO", "O...O", "..O..", "O....", ".O...", "....O", "...O.", ".....", ".....", ".....", "O...O", "O...O", "O...O", "O...O", "O....", ".O...", "O...O", "O...O", "..O..", "...O.", "O..O.", "..O..", "O.O.O", "O...O", "O...O", "O...O", "O...O", "O....", "....O", ".O...", "O...O", ".O.O.", "O.O.O", ".O.O.", "..O..", ".O...", ".O...", "..O..", "...O.", ".....", 
	".....", "..O..", ".....", ".O.O.", "..O..", "O..OO", ".OO.O", ".....", "..O..", "..O..", "..O..", ".....", "..O..", ".....", ".....", ".....", ".OOO.", "OOOOO", "OOOOO", ".OOO.", "...O.", ".OOO.", ".OOO.", "..O..", ".OOO.", ".OOO.", ".....", ".O...", "...O.", ".....", ".O...", "..O..", ".OOOO", "O...O", "OOOO.", ".OOO.", "OOOO.", "OOOOO", "O....", ".OOO.", "O...O", "OOOOO", ".OO..", "O...O", "OOOOO", "O...O", "O...O", ".OOO.", "O....", ".OO.O", "O...O", ".OOO.", "..O..", ".OOOO", "..O..", "O...O", "O...O", "..O..", "OOOOO", ".OOO.", ".....", ".OOO.", ".....", ".....", ".....", ".OOOO", "OOOO.", ".OOO.", ".OOOO", ".OOOO", ".O...", ".OOOO", "O...O", ".OOO.", "...O.", "O...O", ".OOO.", "O.O.O", "O...O", ".OOO.", "OOOO.", ".OOOO", "O....", "OOOO.", "..OO.", ".OOOO", "..O..", "OO.OO", "O...O", ".O...", "OOOOO", "..OO.", "..O..", ".OO..", ".....", 
	".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", "...O.", ".O...", ".....", ".....", "..O..", ".....", "..O..", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", "....O", ".....", ".....", "O..O.", ".....", ".....", ".....", ".....", ".....", "O....", "....O", ".....", ".....", ".....", ".....", ".....", ".....", ".....", "O....", ".....", ".....", "..O..", ".....", ".....", 
	".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".O...", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", "OOOOO", ".....", ".....", ".....", ".....", ".....", ".....", ".....", "OOOO.", ".....", ".....", ".OO..", ".....", ".....", ".....", ".....", ".....", "O....", "....O", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", ".....", 
};


//------------------------------------------------------------------------------------------------
void AppendGlyphTriangles2D( std::vector<Vertex_PCU>& verts, char glyph, const Vec2& cellMins, const Vec2& pixelSize, const Rgba8& color )
{
	if( glyph < TRITEXT_FIRST_ASCII || glyph > TRITEXT_LAST_ASCII )
		return;

	int triTextGlyphIndex = glyph - TRITEXT_FIRST_ASCII;
	for( int rowIndex = 0; rowIndex < TRITEXT_PIX_HIGH; ++ rowIndex )
	{
		float minY = cellMins.y + pixelSize.y * (float)(TRITEXT_PIX_HIGH - rowIndex - 1);
		float maxY = minY + pixelSize.y;
		int triTextFontDataIndex = triTextGlyphIndex + (rowIndex * TRITEXT_NUM_ASCIIS);
		const char* rowText = g_triTextFontData[ triTextFontDataIndex ];

		for( int triTextCellIndex = 0; triTextCellIndex < TRITEXT_PIX_WIDE; ++ triTextCellIndex )
		{
			if( rowText[ triTextCellIndex ] == '.' )
				continue;

			float minX = cellMins.x + pixelSize.x * (float) triTextCellIndex;
			float maxX = minX + pixelSize.x;

			AppendVertsForAABB2D( verts, AABB2( minX,minY, maxX,maxY ), Vec2::ZERO, Vec2::ONE, color );
		}
	}
};


//------------------------------------------------------------------------------------------------
void DrawTextTriangles2D( RenderContext& renderer, const std::string& text, const Vec2& startMins, float cellHeight, const Rgba8& color, float cellAspect, float spacingFraction )
{
	std::vector<Vertex_PCU> verts;
	AppendTextTriangles2D( verts, text, startMins, cellHeight, color, cellAspect, spacingFraction );
	renderer.DrawVertexArray( verts );
}


//------------------------------------------------------------------------------------------------
void AppendTextTriangles2D( std::vector<Vertex_PCU>& verts, const std::string& text, const Vec2& startMins, float cellHeight, const Rgba8& color, float cellAspect, float spacingFraction )
{
	size_t estimatedNumberOfNewVerts = text.length() * TRITEXT_PIX_WIDE * TRITEXT_PIX_HIGH * (6 / 2); // assumes (liberally) that fewer than half of a glyph's pixels are lit on average
	verts.reserve( verts.size() + estimatedNumberOfNewVerts );
	
	float cellWidth = cellHeight * cellAspect;
	float pixelWidth = cellWidth * (1.f / (float) TRITEXT_PIX_WIDE);
	float pixelHeight = cellHeight * (1.f / (float) TRITEXT_PIX_HIGH);
	float spacingWidth = cellWidth * spacingFraction;
	Vec2 cellSize( cellWidth, cellHeight );
	Vec2 pixelSize( pixelWidth, pixelHeight );
	Vec2 cellMins = startMins;

	int numGlyphs = (int) text.length();
	for( int i = 0; i < numGlyphs; ++ i )
	{
		char glyph = text[i];
		AppendGlyphTriangles2D( verts, glyph, cellMins, pixelSize, color );
		cellMins.x += cellSize.x;
		cellMins.x += spacingWidth;
	}
}


