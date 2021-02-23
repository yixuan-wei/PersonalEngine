#pragma once

#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Math/Mat44.hpp"

struct obj_import_options
{
    Mat44 transform = Mat44::IDENTITY;

    bool smoothNormals = false;
    bool generateNormals = false;
    bool generateTangents = false;
    bool invertWindingOrder = false;
    bool invertVCoord = false;
};

void LoadOBJToIndexedVertexArray(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indices,
    char const* filename, obj_import_options const& options);
void LoadOBJToVertexArray(std::vector<Vertex_PCUTBN>& verts, char const* filename, obj_import_options const& options);

void MeshInvertV(std::vector<Vertex_PCUTBN>& verts);
void MeshCalculateNormal(std::vector<Vertex_PCUTBN>& verts);
void MeshSmoothNormal(std::vector<Vertex_PCUTBN>& verts);
void MeshInvertWindingOrder(std::vector<Vertex_PCUTBN>& verts);
void MeshInvertIndexWindingOrder(std::vector<unsigned int>& indices);
void MeshGenerateTangents(std::vector<Vertex_PCUTBN>& verts);

void CleanVertexesForIndexedVertexArray(std::vector<Vertex_PCUTBN> const& rawVerts,
    std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indices);
