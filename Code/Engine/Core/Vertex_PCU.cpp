#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/buffer_attribute_t.hpp"

buffer_attribute_t const Vertex_PCU::LAYOUT[] =
{
    buffer_attribute_t("POSITION",  eBufferFormatType::BUFFER_FORMAT_VEC3,      		offsetof(Vertex_PCU, position)),
    buffer_attribute_t("COLOR",     eBufferFormatType::BUFFER_FORMAT_R8G8B8A8_UNORM, offsetof(Vertex_PCU, tint)),
    buffer_attribute_t("TEXCOORD",  eBufferFormatType::BUFFER_FORMAT_VEC2,      		offsetof(Vertex_PCU, uvTexCoords)),
    buffer_attribute_t() // end - terminator element; 
};

//////////////////////////////////////////////////////////////////////////
Vertex_PCU::Vertex_PCU( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoord /* = Vec2(0.f, 0.f) */ )
	: position(position)
	, tint(tint)
	, uvTexCoords(uvTexCoord)
{
} 

//////////////////////////////////////////////////////////////////////////
Vertex_PCU::Vertex_PCU( const Vec2& position, const Rgba8& tint, const Vec2& uvTexCoord /*= Vec2( 0.f, 0.f ) */ )
	: position( Vec3(position.x, position.y, 0.f) )
	, tint(tint)
	, uvTexCoords(uvTexCoord)
{
}

//////////////////////////////////////////////////////////////////////////
Vertex_PCU GetMiddleVertex(Vertex_PCU const& VPA, Vertex_PCU const& VPB)
{
	Vec3 position = (VPA.position + VPB.position) * .5f;
	Vec2 uv = (VPA.uvTexCoords + VPB.uvTexCoords) * .5f;
	Rgba8 color = Lerp(VPA.tint, VPB.tint);
	return Vertex_PCU(position,color,uv);
}

//////////////////////////////////////////////////////////////////////////
void TransformVertexArray( int vertexesNumber, Vertex_PCU* vertexes, float uniformScale, float rotationDegrees, const Vec2& translationXY )
{
	//construct the local coordinates in world coords
	Vec2 iLocal = Vec2::MakeFromPolarDegrees( rotationDegrees, uniformScale );
	Vec2 jLocal( -iLocal.y, iLocal.x );

	for( int vertexIndex = 0; vertexIndex < vertexesNumber; vertexIndex++ )
	{
		Vertex_PCU& vert = vertexes[vertexIndex];

		Vec2 newPosXY = vert.position.x * iLocal + vert.position.y * jLocal + translationXY;
		vert.position.x = newPosXY.x;
		vert.position.y = newPosXY.y;
	}
}

//////////////////////////////////////////////////////////////////////////
void TransformVertexArray( int vertexesNumber, Vertex_PCU* vertexes, float uniformScale, float rotationDegrees, const Vec2& translationXY, const Rgba8& color )
{
	//construct the local coordinates in world coords
	Vec2 iLocal = Vec2::MakeFromPolarDegrees( rotationDegrees, uniformScale );
	Vec2 jLocal( -iLocal.y, iLocal.x );

	for( int vertexIndex = 0; vertexIndex < vertexesNumber; vertexIndex++ )
	{
		Vertex_PCU& vert = vertexes[vertexIndex];

		Vec2 newPosXY = vert.position.x * iLocal + vert.position.y * jLocal + translationXY;
		vert.position.x = newPosXY.x;
		vert.position.y = newPosXY.y;
		vert.tint = color;
	}
}

//////////////////////////////////////////////////////////////////////////
void TransformVertexArray(int vertexesNumber, Vertex_PCU* vertexes, Mat44 const& transformMat)
{
	for (int i = 0; i < vertexesNumber; i++) {
		Vertex_PCU& vert = vertexes[i];

		vert.position = transformMat.TransformPosition3D(vert.position);
	}
}

//////////////////////////////////////////////////////////////////////////
void AppendVertsForLine2D( std::vector<Vertex_PCU>& verts, const Vec2& startPos, const Vec2& endPos, float thickness, const Rgba8& color )
{
	Vec2 forward = (endPos - startPos).GetNormalized();
	Vec2 vertical = forward.GetRotated90Degrees();

	float length = thickness * .5f;
	Vec2 endUp = endPos + (forward + vertical) * length;
	Vec2 endDown = endPos + (forward - vertical) * length;
	Vec2 startUp = startPos + (vertical - forward) * length;
	Vec2 startDown = startPos + (-vertical - forward) * length;

	//right down triangle
	verts.push_back(Vertex_PCU( startDown, color ));
	verts.push_back( Vertex_PCU( endDown, color ) );
	verts.push_back( Vertex_PCU( endUp, color ));

	//lef.push_back(triangle			   
	verts.push_back( Vertex_PCU( startDown, color ));
	verts.push_back( Vertex_PCU( endUp, color ));
	verts.push_back( Vertex_PCU( startUp, color ));
}

//////////////////////////////////////////////////////////////////////////
void AppendVertsForLine2D( std::vector<Vertex_PCU>& verts, const LineSegment2& line, float thickness, const Rgba8& color )
{
	AppendVertsForLine2D( verts, line.start, line.end, thickness, color );
}

//////////////////////////////////////////////////////////////////////////
void AppendVertsForAABBOutline2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, float thickness, Rgba8 const& color)
{
    Vec2 mins = bounds.mins;
    Vec2 maxs = bounds.maxs;
	Vec2 leftRight(maxs.x, mins.y);
	Vec2 upLeft(mins.x, maxs.y);
	AppendVertsForLine2D(verts, mins, leftRight, thickness, color);
	AppendVertsForLine2D(verts, leftRight, maxs, thickness, color);
	AppendVertsForLine2D(verts, upLeft, maxs, thickness, color);
	AppendVertsForLine2D(verts, upLeft, mins, thickness, color);
}

//////////////////////////////////////////////////////////////////////////
void AppendVertsForRing2D( std::vector<Vertex_PCU>& verts, const Vec2& center, float radius, float thickness, const Rgba8& color )
{
	Vec2* outterRingVerts = new Vec2[CIRCLE_FRAGMENT_NUM];
	Vec2* innerRingVerts = new Vec2[CIRCLE_FRAGMENT_NUM];
	float unitDegrees = 360.f / (float)CIRCLE_FRAGMENT_NUM;
	float halfThick = thickness * .5f;
	float cosine = CosDegrees(unitDegrees);
	float sine = SinDegrees(unitDegrees);
	Vec2 outPointer( radius + halfThick, 0.f );
	Vec2 iOutLocal = Vec2(cosine,-sine);
	Vec2 jOutLocal( -iOutLocal.y, iOutLocal.x );
	Vec2 inPointer( radius - halfThick, 0.f );
	Vec2 iInLocal = Vec2(cosine,-sine);
	Vec2 jInLocal( -iInLocal.y, iInLocal.x );

	for( int vertIndex = 0; vertIndex < CIRCLE_FRAGMENT_NUM; vertIndex++ )
	{
		outterRingVerts[vertIndex] = center + outPointer;
		innerRingVerts[vertIndex] = center + inPointer;
		outPointer = outPointer.x * iOutLocal + outPointer.y * jOutLocal;
		inPointer = inPointer.x * iInLocal + inPointer.y * jInLocal;
	}

	for( int vertIndex = 0; vertIndex < CIRCLE_FRAGMENT_NUM - 1; vertIndex++ )
	{
		Vec2& innerBig = innerRingVerts[vertIndex + 1];
		Vec2& outterSmall = outterRingVerts[vertIndex];

		//Init triangle inner small
        verts.push_back(Vertex_PCU(innerBig, color));
		verts.push_back(Vertex_PCU( outterSmall, color ));
        verts.push_back(Vertex_PCU(innerRingVerts[vertIndex], color));

		//init triangle outter big
		verts.push_back(Vertex_PCU( innerBig, color ));
		verts.push_back(Vertex_PCU( outterRingVerts[vertIndex + 1], color ));
        verts.push_back(Vertex_PCU(outterSmall, color));
	}

	//deal with last fragment
	Vec2& innerBig = innerRingVerts[0];
	Vec2& outterSmall = outterRingVerts[CIRCLE_FRAGMENT_NUM - 1];

	//Init triangle inner small
    verts.push_back(Vertex_PCU(innerBig, color));
    verts.push_back(Vertex_PCU(outterSmall, color));
    verts.push_back(Vertex_PCU(innerRingVerts[CIRCLE_FRAGMENT_NUM - 1], color));

	//init triangle outter big
    verts.push_back(Vertex_PCU(innerBig, color));
    verts.push_back(Vertex_PCU(outterRingVerts[0], color));
	verts.push_back(Vertex_PCU( outterSmall, color ));

	delete[] outterRingVerts;
	delete[] innerRingVerts;
}

//////////////////////////////////////////////////////////////////////////
void AppendVertsForDisc2D( std::vector<Vertex_PCU>& verts, const Vec2& position, float radius, const Rgba8& color, int fragmentNum /*=CIRCLE_FRAGMENT_NUM*/)
{
	float unitDegrees = 360.f /(float)fragmentNum;
	Vec2* ringVerts = new Vec2[fragmentNum];

	for( int vertID = 0; vertID < fragmentNum; vertID++ )
	{
		ringVerts[vertID] = position + Vec2::MakeFromPolarDegrees( unitDegrees * (float)vertID, radius );
	}

	for( int vertID = 0; vertID < fragmentNum; vertID++ )
	{
		verts.push_back( Vertex_PCU( position, color ) );
		verts.push_back( Vertex_PCU( ringVerts[vertID], color ) );

		if( vertID == fragmentNum - 1 )
		{
			verts.push_back( Vertex_PCU( ringVerts[0], color ) );
		}
		else
		{
			verts.push_back( Vertex_PCU( ringVerts[vertID + 1], color ) );
		}
	}

	delete[] ringVerts;
}

//////////////////////////////////////////////////////////////////////////
void AppendVertsForAABB2D( std::vector<Vertex_PCU>& verts, const AABB2& bounds, const Vec2& uvCoordsMins/*=zero*/, const Vec2& uvCoordsMaxs/*=one*/, const Rgba8& color /*=white*/)
{
	Vec2 mins = bounds.mins;
	Vec2 maxs = bounds.maxs;
	Vertex_PCU bottomLeft = Vertex_PCU( mins, color,uvCoordsMins );
	Vertex_PCU upperRight = Vertex_PCU( maxs, color, uvCoordsMaxs );

	verts.push_back( bottomLeft );
	verts.push_back( Vertex_PCU( Vec2( maxs.x, mins.y ), color, Vec2(uvCoordsMaxs.x,uvCoordsMins.y) ));
	verts.push_back( upperRight );

	verts.push_back( bottomLeft );
	verts.push_back( upperRight );
	verts.push_back( Vertex_PCU( Vec2( mins.x, maxs.y ), color, Vec2(uvCoordsMins.x,uvCoordsMaxs.y) ) );
}

//////////////////////////////////////////////////////////////////////////
void AppendVertsForCapsule2D( std::vector<Vertex_PCU>& verts, const Capsule2& capsule, const Rgba8& color, int fragmentNum )
{
	Vec2 startToEnd = capsule.boneEnd - capsule.boneStart;
	Vec2 radiusUp( -startToEnd.y, startToEnd.x );
	radiusUp.SetLength( capsule.radius );
	AABB2 bone( capsule.boneStart - radiusUp, capsule.boneEnd + radiusUp);
	AppendVertsForAABB2D( verts, bone, Vec2::ZERO, Vec2::ONE, color );

	float unitDegrees = 360.f / (float)fragmentNum;
	Vec2 iLocal = Vec2::MakeFromPolarDegrees( unitDegrees);
	Vec2 jLocal( -iLocal.y, iLocal.x );
	Vec2 lastPoint = radiusUp;
	Vec2 curPoint = radiusUp;
	int halfPointsNum = (int)((float)fragmentNum * .5f);

	//draw start semi circle
	for( int pIdx = 0; pIdx < halfPointsNum; pIdx++ )
	{
		curPoint = curPoint.x * iLocal + curPoint.y * jLocal;
		verts.push_back( Vertex_PCU( capsule.boneStart, color ) );
		verts.push_back( Vertex_PCU( capsule.boneStart + lastPoint, color ));
		verts.push_back( Vertex_PCU( capsule.boneStart + curPoint, color ) );
		lastPoint = curPoint;
	}
	verts.push_back( Vertex_PCU( capsule.boneStart, color ) );
	verts.push_back( Vertex_PCU( capsule.boneStart + curPoint, color ) );
	verts.push_back( Vertex_PCU( capsule.boneStart - radiusUp, color ) );

	//draw end semi circle
	curPoint = -radiusUp; 
	lastPoint = -radiusUp;
	for( int pIdx = 0; pIdx < halfPointsNum; pIdx++ )
	{
		curPoint = curPoint.x * iLocal + curPoint.y * jLocal;
		verts.push_back( Vertex_PCU( capsule.boneEnd, color ) );
		verts.push_back( Vertex_PCU( capsule.boneEnd + lastPoint, color ) );
		verts.push_back( Vertex_PCU( capsule.boneEnd + curPoint, color ) );
		lastPoint = curPoint;
	}
	verts.push_back( Vertex_PCU( capsule.boneEnd, color ) );
	verts.push_back( Vertex_PCU( capsule.boneEnd + curPoint, color ) );
	verts.push_back( Vertex_PCU( capsule.boneEnd + radiusUp, color ) );
}

//////////////////////////////////////////////////////////////////////////
void AppendVertsForPolygon2D( std::vector<Vertex_PCU>& verts, Polygon2D const& polygon, const Rgba8& color )
{
	if( !polygon.IsValid() )
	{
		return;
	}

	Vec2 first, second;
	polygon.GetEdge( 0, &first, &second );
	int edgeCount = polygon.GetEdgeCount();
	for( int idx = 1; idx < edgeCount; idx++ )
	{
		Vec2 start, end;
		polygon.GetEdge( idx, &start, &end );
		verts.push_back( Vertex_PCU( first, color ) );
		verts.push_back( Vertex_PCU( start, color ) );
		verts.push_back( Vertex_PCU( end, color ) );
	}
}
