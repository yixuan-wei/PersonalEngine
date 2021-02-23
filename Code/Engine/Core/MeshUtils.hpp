#pragma once

#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Plane2D.hpp"

void AppendIndexedVertexesForAABB3D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    AABB3 const& aabb3, Rgba8 const& color = Rgba8::WHITE, Vec2 const& uvMins = Vec2::ZERO, Vec2 const& uvMaxs = Vec2::ONE); 
void AppendIndexedVertexesForAABB3D(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices,
        AABB3 const& aabb3, Rgba8 const& color = Rgba8::WHITE, Vec2 const& uvMins = Vec2::ZERO, Vec2 const& uvMaxs = Vec2::ONE);

void AppendIndexedVertexesForQuaterPolygon2D(std::vector<Vertex_PCU>& vertexes, 
    Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft,
    Rgba8 const& color = Rgba8::WHITE, Vec2 const& uvMins = Vec2::ZERO, Vec2 const& uvMaxs = Vec2::ONE);
void AppendIndexedVertexesForQuaterPolygon2D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft,
    Rgba8 const& color = Rgba8::WHITE, Vec2 const& uvMins = Vec2::ZERO, Vec2 const& uvMaxs = Vec2::ONE); 
void AppendIndexedVertexesForQuaterPolygon2D(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices,
        Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft,
        Rgba8 const& color = Rgba8::WHITE, Vec2 const& uvMins = Vec2::ZERO, Vec2 const& uvMaxs = Vec2::ONE);

void AppendIndexedVertexesForXYPlane2D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    Vec3 const& planeCenter, Vec2 const& planeSize, unsigned int horizontalCuts = 10, unsigned int verticalCuts = 10,
    Rgba8 const& color = Rgba8::WHITE, Vec2 const& uvMins = Vec2::ZERO, Vec2 const& uvMaxs = Vec2::ONE);
void AppendIndexedVertexesForXYPlane2D(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices,
    Vec3 const& planeCenter, Vec2 const& planeSize, unsigned int horizontalCuts = 10, unsigned int verticalCuts = 10,
    Rgba8 const& color = Rgba8::WHITE, Vec2 const& uvMins = Vec2::ZERO, Vec2 const& uvMaxs = Vec2::ONE);

void AppendIndexedVertexesForUVSphere(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    Vec3 const& center, float radius, unsigned int horizontalCuts = CIRCLE_FRAGMENT_NUM, unsigned int verticalCuts = CIRCLE_FRAGMENT_NUM, 
    Rgba8 const& color = Rgba8::WHITE, Vec2 const& uvMins = Vec2::ZERO, Vec2 const& uvMaxs = Vec2::ONE);
void AppendIndexedVertexesForUVSphere(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices,
    Vec3 const& center, float radius, unsigned int horizontalCuts = CIRCLE_FRAGMENT_NUM, unsigned int verticalCuts = CIRCLE_FRAGMENT_NUM,
    Rgba8 const& color = Rgba8::WHITE, Vec2 const& uvMins = Vec2::ZERO, Vec2 const& uvMaxs = Vec2::ONE);

void AppendIndexedVertexesForCubeSphere(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    Vec3 const& center, float radius, unsigned int tesselationCount = 4, Rgba8 const& color = Rgba8::WHITE,
    Vec2 const& uvMins = Vec2::ZERO, Vec2 const& uvMaxs = Vec2::ONE); 
void AppendIndexedVertexesForCubeSphere(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices,
        Vec3 const& center, float radius, unsigned int tesselationCount = 4, Rgba8 const& color = Rgba8::WHITE,
        Vec2 const& uvMins = Vec2::ZERO, Vec2 const& uvMaxs = Vec2::ONE);

void AppendIndexedVertexesForIcoSphere(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    Vec3 const& center, float radius, unsigned int subdivision = 4, Rgba8 const& color = Rgba8::WHITE);

void AppendIndexedVertexesForZUpCylinder(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    Vec3 const& localCenter, float bottomRadius, float upRadius, float length, unsigned int verticalCuts = CIRCLE_FRAGMENT_NUM,
    Rgba8 const& color = Rgba8::WHITE);

void AppendIndexedVertexesForCylinder(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    Vec3 const& startCenter, Vec3 const& endCenter, float radius, unsigned int verticalCuts = CIRCLE_FRAGMENT_NUM, Rgba8 const& color = Rgba8::WHITE);

void AppendIndexedVertexesForCone(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    Vec3 const& bottomCenter, float radius, float height, Vec3 const& upDirection, unsigned int verticalCuts = CIRCLE_FRAGMENT_NUM,
    Rgba8 const& color = Rgba8::WHITE);

void AppendIndexedVertexesForArrow(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    Vec3 const& start, Vec3 const& end, float baseWidth, float maxWidth, unsigned int verticalCuts = CIRCLE_FRAGMENT_NUM,
    Rgba8 const& color = Rgba8::WHITE);

void AppendIndexedVertexesForTriangle2D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    Vec3 const& p0, Vec3 const& p1, Vec3 const& p2, Rgba8 const& color = Rgba8::WHITE);

void AppendIndexedVertexesForDisc2D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    Vec2 const& center, float radius, Rgba8 const& color = Rgba8::WHITE, unsigned int fragmentNum = 32);

void AppendIndexedVertexesForLine2D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    Vec2 const& start, Vec2 const& end, float width, Rgba8 const& color = Rgba8::WHITE);

void Tesselate(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices);
void Tesselate(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices);
