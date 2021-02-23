#pragma once

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"

struct buffer_attribute_t;
struct Mat44;
struct Vertex_PCUTBN;

Vertex_PCUTBN GetMiddleVertex(Vertex_PCUTBN const& VPA, Vertex_PCUTBN const& VPB);

void TransformVertexArray(int vertexesNumber, Vertex_PCUTBN* vertexes, Mat44 const& transformMat);
void TransformVertexArray(int vertexesNumber, Vertex_PCUTBN* vertexes, float uniformScale, float rotationAboutZDegrees, Vec3 const& translation);
void TransformVertexArray(int vertexesNumber, Vertex_PCUTBN* vertexes, float uniformScale, float rotationAboutZDegrees, Vec3 const& translation, Rgba8 const& color);

//////////////////////////////////////////////////////////////////////////
struct Vertex_PCUTBN
{
public:
    Vec3  position;
    Rgba8 tint;
    Vec2  uvTexCoords;

    Vec4 tangent = Vec4(1.f, 0.f, 0.f, 1.f);
    Vec3 normal = Vec3(0.f, 0.f, 1.f);

    static buffer_attribute_t const LAYOUT[];

public:
    Vertex_PCUTBN() = default;
    explicit Vertex_PCUTBN(Vec3 const& pos, Rgba8 const& color, Vec2 const& uv = Vec2::ZERO,
        Vec4 const& t = Vec4(1.f, 0.f, 0.f, 1.f), Vec3 const& n = Vec3(0.f, 0.f, 1.f));

};