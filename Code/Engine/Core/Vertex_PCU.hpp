#pragma once

#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <vector>

//standalone vertex helper function
struct Vertex_PCU;
struct Capsule2;
struct LineSegment2;
struct Mat44;
struct AABB2;
class Polygon2D;
struct buffer_attribute_t;

Vertex_PCU GetMiddleVertex(Vertex_PCU const& VPA, Vertex_PCU const& VPB);

void TransformVertexArray(int vertexesNumber, Vertex_PCU* vertexes, Mat44 const& transformMat);
void TransformVertexArray( int vertexesNumber, Vertex_PCU* vertexes, float uniformScale, float rotationDegrees, const Vec2& translationXY );//Just considering the XY values in position
void TransformVertexArray( int vertexesNumber, Vertex_PCU* vertexes, float uniformScale, float rotationDegrees, const Vec2& translationXY, const Rgba8& color );//Just considering the XY values in position
void AppendVertsForLine2D( std::vector<Vertex_PCU>& verts, const Vec2& startPos, const Vec2& endPos, float thickness, const Rgba8& color );
void AppendVertsForAABBOutline2D(std::vector<Vertex_PCU>& verts, AABB2 const& bounds, float thickness, Rgba8 const& color);
void AppendVertsForLine2D( std::vector<Vertex_PCU>& verts, const LineSegment2& line, float thickness, const Rgba8& color );
void AppendVertsForRing2D( std::vector<Vertex_PCU>& verts, const Vec2& position, float radius, float thickness, const Rgba8& color );
void AppendVertsForDisc2D( std::vector<Vertex_PCU>& verts, const Vec2& position, float radius, const Rgba8& color, int fragmentNum = CIRCLE_FRAGMENT_NUM );
void AppendVertsForAABB2D( std::vector<Vertex_PCU>& verts, const AABB2& bounds,  const Vec2& uvCoordsMins = Vec2::ZERO, const Vec2& uvCoordsMaxs = Vec2::ONE, const Rgba8& color = Rgba8::WHITE);//the uv texture points are currently (0,0) and (1,1)
void AppendVertsForCapsule2D( std::vector<Vertex_PCU>& verts, const Capsule2& capsule, const Rgba8& color, int fragmentNum = CIRCLE_FRAGMENT_NUM );
void AppendVertsForPolygon2D( std::vector<Vertex_PCU>& verts, Polygon2D const& polygon, const Rgba8& color );

//////////////////////////////////////////////////////////////////////////
struct Vertex_PCU 
{
public:
	Vec3  position;
	Rgba8 tint;
	Vec2  uvTexCoords;

	static buffer_attribute_t const LAYOUT[];

public:
	Vertex_PCU(){}
	explicit Vertex_PCU( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoord = Vec2( 0.f, 0.f ) );	
	explicit Vertex_PCU( const Vec2& position, const Rgba8& tint, const Vec2& uvTexCoord = Vec2( 0.f, 0.f ) );//explicit constructor for 2D position
};