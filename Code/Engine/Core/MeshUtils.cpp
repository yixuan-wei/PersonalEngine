#include "Engine/Core/MeshUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <cmath>


//////////////////////////////////////////////////////////////////////////
static void AppendIndexesForCuts(std::vector<unsigned int>& indices, unsigned int horizontalCuts, unsigned int verticalCuts, unsigned int vertStartIdx=0)
{
    for (unsigned int hIdx = 0; hIdx < horizontalCuts; hIdx++) {
        for (unsigned int vIdx = 0; vIdx < verticalCuts; vIdx++) {
            unsigned int startIndice = vIdx * (horizontalCuts + 1) + hIdx + vertStartIdx;

            indices.push_back(startIndice);
            indices.push_back(startIndice + 1);
            indices.push_back(startIndice + horizontalCuts + 2);
            indices.push_back(startIndice);
            indices.push_back(startIndice + horizontalCuts + 2);
            indices.push_back(startIndice + horizontalCuts + 1);
        }
    }
}


//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForAABB3D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices, AABB3 const& aabb3, 
    Rgba8 const& color /*= Rgba8::WHITE*/, Vec2 const& uvMins /*= Vec2::ZERO*/, Vec2 const& uvMaxs /*= Vec2::ONE*/)
{
    Vec3 corners[8];
    aabb3.GetCornerPoints(&corners[0]);

    //                                                  bLeft         bRight      tRight      tLeft
    AppendIndexedVertexesForQuaterPolygon2D(vertexes, indices, corners[1], corners[0], corners[4], corners[5], color, uvMins, uvMaxs); // front
    AppendIndexedVertexesForQuaterPolygon2D(vertexes, indices, corners[3], corners[2], corners[6], corners[7], color, uvMins, uvMaxs); // back
    AppendIndexedVertexesForQuaterPolygon2D(vertexes, indices, corners[0], corners[3], corners[7], corners[4], color, uvMins, uvMaxs); // left
    AppendIndexedVertexesForQuaterPolygon2D(vertexes, indices, corners[2], corners[1], corners[5], corners[6], color, uvMins, uvMaxs); // right
    AppendIndexedVertexesForQuaterPolygon2D(vertexes, indices, corners[7], corners[6], corners[5], corners[4], color, uvMins, uvMaxs); // top
    AppendIndexedVertexesForQuaterPolygon2D(vertexes, indices, corners[0], corners[1], corners[2], corners[3], color, uvMins, uvMaxs); // bottom
}


//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForAABB3D(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices, AABB3 const& aabb3, Rgba8 const& color /*= Rgba8::WHITE*/, Vec2 const& uvMins /*= Vec2::ZERO*/, Vec2 const& uvMaxs /*= Vec2::ONE*/)
{
    Vec3 corners[8];
    aabb3.GetCornerPoints(&corners[0]);

    //                                                  bLeft         bRight      tRight      tLeft
    AppendIndexedVertexesForQuaterPolygon2D(vertexes, indices, corners[1], corners[0], corners[4], corners[5], color, uvMins, uvMaxs); // front
    AppendIndexedVertexesForQuaterPolygon2D(vertexes, indices, corners[3], corners[2], corners[6], corners[7], color, uvMins, uvMaxs); // back
    AppendIndexedVertexesForQuaterPolygon2D(vertexes, indices, corners[0], corners[3], corners[7], corners[4], color, uvMins, uvMaxs); // left
    AppendIndexedVertexesForQuaterPolygon2D(vertexes, indices, corners[2], corners[1], corners[5], corners[6], color, uvMins, uvMaxs); // right
    AppendIndexedVertexesForQuaterPolygon2D(vertexes, indices, corners[7], corners[6], corners[5], corners[4], color, uvMins, uvMaxs); // top
    AppendIndexedVertexesForQuaterPolygon2D(vertexes, indices, corners[0], corners[1], corners[2], corners[3], color, uvMins, uvMaxs); // bottom
}


//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForUVSphere(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices, Vec3 const& center, float radius, 
    unsigned int horizontalCuts /*= CIRCLE_FRAGMENT_NUM*/, unsigned int verticalCuts /*= CIRCLE_FRAGMENT_NUM*/, Rgba8 const& color /*= Rgba8::WHITE*/,
    Vec2 const& uvMins /*ZERO*/, Vec2 const& uvMaxs /*ONE*/)
{
    if (horizontalCuts <= 2 || verticalCuts <= 1) {
        return;
    }

    unsigned int vertStartIdx = (unsigned int)vertexes.size();
    float unitU = (uvMaxs.x-uvMins.x) / (float)horizontalCuts;
    float unitV = (uvMaxs.y-uvMins.y) / (float)verticalCuts;
    float unitHorizontalDegrees = 360.f / (float)horizontalCuts;
    float unitVerticalDegrees = 180.f / (float)verticalCuts;

    //generate vertexes on rectangle sheet
    for (unsigned int vIdx = 0; vIdx < verticalCuts + 1; vIdx++) {
        float vDegrees = unitVerticalDegrees * (float)vIdx - 90.f;
        float v = unitV * (float)vIdx + uvMins.y;
        Vec2 vDirection = Vec2::MakeFromPolarDegrees(vDegrees);

        for (unsigned int hIdx = 0; hIdx < horizontalCuts + 1; hIdx++) {
            float hDegrees = 360.f - unitHorizontalDegrees * (float)hIdx;
            float u = unitU * (float)hIdx;
            Vec2 hDirection = Vec2::MakeFromPolarDegrees(hDegrees);

            float x = radius * hDirection.x * vDirection.x;
            float y = radius * vDirection.y;
            float z = radius* hDirection.y* vDirection.x;

            vertexes.push_back(Vertex_PCU(Vec3(x, y, z)+center, color, Vec2(u, v)));
        }
    }

    AppendIndexesForCuts(indices, horizontalCuts, verticalCuts, vertStartIdx);
}


//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForUVSphere(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices, 
    Vec3 const& center, float radius, unsigned int horizontalCuts /*= CIRCLE_FRAGMENT_NUM*/, 
    unsigned int verticalCuts /*= CIRCLE_FRAGMENT_NUM*/, Rgba8 const& color /*= Rgba8::WHITE*/, 
    Vec2 const& uvMins /*= Vec2::ZERO*/, Vec2 const& uvMaxs /*= Vec2::ONE*/)
{
    if (horizontalCuts <= 2 || verticalCuts <= 1) {
        return;
    }

    unsigned int vertStartIdx = (unsigned int)vertexes.size();
    float unitU = (uvMaxs.x - uvMins.x) / (float)horizontalCuts;
    float unitV = (uvMaxs.y - uvMins.y) / (float)verticalCuts;
    float unitHorizontalDegrees = 360.f / (float)horizontalCuts;
    float unitVerticalDegrees = 180.f / (float)verticalCuts;

    //generate vertexes on rectangle sheet
    for (unsigned int vIdx = 0; vIdx < verticalCuts + 1; vIdx++) {
        float vDegrees = unitVerticalDegrees * (float)vIdx - 90.f;
        float v = unitV * (float)vIdx + uvMins.y;
        Vec2 vDirection = Vec2::MakeFromPolarDegrees(vDegrees);

        for (unsigned int hIdx = 0; hIdx < horizontalCuts + 1; hIdx++) {
            float hDegrees = 360.f - unitHorizontalDegrees * (float)hIdx;
            float u = unitU * (float)hIdx+uvMins.x;
            Vec2 hDirection = Vec2::MakeFromPolarDegrees(hDegrees);

            float x = hDirection.x * vDirection.x;
            float y = vDirection.y;
            float z = hDirection.y * vDirection.x;

            Vec3 centerToP(x, y, z);
            Vec3 tangent(hDirection.y, 0.f, -hDirection.x);

            vertexes.push_back(Vertex_PCUTBN(centerToP * radius + center, color, Vec2(u, v), Vec4(tangent, 1.f),
                centerToP.GetNormalized()));
        }
    }

    AppendIndexesForCuts(indices, horizontalCuts, verticalCuts, vertStartIdx);
}


//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForQuaterPolygon2D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices, 
    Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft,
    Rgba8 const& color /*= Rgba8::WHITE*/, Vec2 const& uvMins /*= Vec2::ZERO*/, Vec2 const& uvMaxs /*= Vec2::ONE*/)
{
    unsigned int startIdx = (unsigned int)vertexes.size();
    vertexes.push_back(Vertex_PCU(bottomLeft, color, uvMins));
    vertexes.push_back(Vertex_PCU(bottomRight, color, Vec2(uvMaxs.x, uvMins.y)));
    vertexes.push_back(Vertex_PCU(topRight, color, uvMaxs));
    vertexes.push_back(Vertex_PCU(topLeft, color, Vec2(uvMins.x, uvMaxs.y)));

    indices.push_back(startIdx);
    indices.push_back(startIdx + 1);
    indices.push_back(startIdx + 2);
    indices.push_back(startIdx);
    indices.push_back(startIdx + 2);
    indices.push_back(startIdx + 3);
}


//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForQuaterPolygon2D(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color /*= Rgba8::WHITE*/, Vec2 const& uvMins /*= Vec2::ZERO*/, Vec2 const& uvMaxs /*= Vec2::ONE*/)
{
    unsigned int startIdx = (unsigned int)vertexes.size();
    Vec3 BLToBR = (bottomRight - bottomLeft).GetNormalized();
    Vec3 BLToTL = (topLeft - bottomLeft).GetNormalized();
    Vec3 BRToTR = (topRight - bottomRight).GetNormalized();
    Vec3 TLToTR = (topRight - topLeft).GetNormalized();
    vertexes.push_back(Vertex_PCUTBN(bottomLeft,    color, uvMins,                      Vec4(BLToBR,1.f), CrossProduct3D(BLToBR,BLToTL)));
    vertexes.push_back(Vertex_PCUTBN(bottomRight,   color, Vec2(uvMaxs.x, uvMins.y),    Vec4(BLToBR,1.f), CrossProduct3D(BLToBR,BRToTR)));
    vertexes.push_back(Vertex_PCUTBN(topRight,      color, uvMaxs,                      Vec4(TLToTR, 1.f),CrossProduct3D(TLToTR,BRToTR)));
    vertexes.push_back(Vertex_PCUTBN(topLeft,       color, Vec2(uvMins.x, uvMaxs.y),    Vec4(TLToTR,1.f), CrossProduct3D(TLToTR,BLToTL)));

    indices.push_back(startIdx);
    indices.push_back(startIdx + 1);
    indices.push_back(startIdx + 2);
    indices.push_back(startIdx);
    indices.push_back(startIdx + 2);
    indices.push_back(startIdx + 3);
}


//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForQuaterPolygon2D(std::vector<Vertex_PCU>& vertexes, Vec3 const& bottomLeft, Vec3 const& bottomRight, Vec3 const& topRight, Vec3 const& topLeft, Rgba8 const& color /*= Rgba8::WHITE*/, Vec2 const& uvMins /*= Vec2::ZERO*/, Vec2 const& uvMaxs /*= Vec2::ONE*/)
{
    vertexes.push_back(Vertex_PCU(bottomLeft, color, uvMins));
    vertexes.push_back(Vertex_PCU(bottomRight, color, Vec2(uvMaxs.x, uvMins.y)));
    vertexes.push_back(Vertex_PCU(topRight, color, uvMaxs));
    vertexes.push_back(Vertex_PCU(bottomLeft, color, uvMins));
    vertexes.push_back(Vertex_PCU(topRight, color, uvMaxs));
    vertexes.push_back(Vertex_PCU(topLeft, color, Vec2(uvMins.x, uvMaxs.y)));
}

//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForXYPlane2D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices, 
    Vec3 const& planeCenter, Vec2 const& planeSize, unsigned int horizontalCuts /*= 10*/, unsigned int verticalCuts /*= 10*/, 
    Rgba8 const& color /*= Rgba8::WHITE*/, Vec2 const& uvMins /*= Vec2::ZERO*/, Vec2 const& uvMaxs /*= Vec2::ONE*/)
{
    if (horizontalCuts == 0 || verticalCuts == 0) {
        return;
    }

    Vec3 halfDim = planeSize*.5f;
    Vec3 mins = planeCenter - halfDim;

    unsigned int vertsStartIdx = (unsigned int)vertexes.size();
    float unitU = (uvMaxs.x-uvMins.x) / (float)horizontalCuts;
    float unitV = (uvMaxs.y-uvMins.y) / (float)verticalCuts;
    float unitHorizontal = planeSize.x / (float)horizontalCuts;
    float unitVertical = planeSize.y / (float)verticalCuts;

    for (unsigned int vIdx = 0; vIdx < verticalCuts+1; vIdx++) {
        float v = unitV * (float)vIdx + uvMins.y;
        float yPos = mins.y + unitVertical*(float)vIdx;

        for (unsigned int hIdx = 0; hIdx < horizontalCuts+1; hIdx++) {
            float u = unitU * (float)hIdx + uvMins.x;
            float xPos = mins.x + unitHorizontal*(float)hIdx;

            vertexes.push_back(Vertex_PCU(Vec3(xPos, yPos, planeCenter.z), color, Vec2(u, v)));
        }
    }

    AppendIndexesForCuts(indices, horizontalCuts, verticalCuts, vertsStartIdx);
}


//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForXYPlane2D(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices, 
    Vec3 const& planeCenter, Vec2 const& planeSize, unsigned int horizontalCuts /*= 10*/, unsigned int verticalCuts /*= 10*/, 
    Rgba8 const& color /*= Rgba8::WHITE*/, Vec2 const& uvMins /*= Vec2::ZERO*/, Vec2 const& uvMaxs /*= Vec2::ONE*/)
{
    if (horizontalCuts == 0 || verticalCuts == 0) {
        return;
    }

    Vec3 halfDim = planeSize * .5f;
    Vec3 mins = planeCenter - halfDim;

    unsigned int vertsStartIdx = (unsigned int)vertexes.size();
    float unitU = (uvMaxs.x - uvMins.x) / (float)horizontalCuts;
    float unitV = (uvMaxs.y - uvMins.y) / (float)verticalCuts;
    float unitHorizontal = planeSize.x / (float)horizontalCuts;
    float unitVertical = planeSize.y / (float)verticalCuts;

    for (unsigned int vIdx = 0; vIdx < verticalCuts + 1; vIdx++) {
        float v = unitV * (float)vIdx + uvMins.y;
        float yPos = mins.y + unitVertical * (float)vIdx;

        for (unsigned int hIdx = 0; hIdx < horizontalCuts + 1; hIdx++) {
            float u = unitU * (float)hIdx + uvMins.x;
            float xPos = mins.x + unitHorizontal * (float)hIdx;

            vertexes.push_back(Vertex_PCUTBN(Vec3(xPos, yPos, planeCenter.z), color, Vec2(u, v)));
        }
    }

    AppendIndexesForCuts(indices, horizontalCuts, verticalCuts, vertsStartIdx);
}


//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForCubeSphere(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices, 
    Vec3 const& center, float radius, unsigned int tesselationCount /*= 4*/, Rgba8 const& color /*= Rgba8::WHITE*/,
    Vec2 const& uvMins /*ZERO*/, Vec2 const& uvMaxs /*ONE*/)
{
    std::vector<Vertex_PCU> sphereVerts;
    std::vector<unsigned int> sphereInds;

    Vec3 halfCube(1.f,1.f,1.f);
    AABB3 cube(center - halfCube, center + halfCube);
    AppendIndexedVertexesForAABB3D(sphereVerts, sphereInds, cube, color, uvMins, uvMaxs);

    for (unsigned int i = 0; i < tesselationCount; i++) {
        Tesselate(sphereVerts, sphereInds);
    }

    //stretch to sphere
    for (size_t i = 0; i < sphereVerts.size(); i++) {
        Vec3 centerToP = sphereVerts[i].position - center;
        centerToP.SetLength(radius);
        sphereVerts[i].position = centerToP + center;
    }

    unsigned int vertStartIdx = (unsigned int)vertexes.size();
    for (size_t i = 0; i < sphereInds.size(); i++) {
        sphereInds[i] += vertStartIdx;
    }

    vertexes.insert(vertexes.end(), sphereVerts.begin(), sphereVerts.end());
    indices.insert(indices.end(), sphereInds.begin(), sphereInds.end());
}


//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForCubeSphere(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices, 
    Vec3 const& center, float radius, unsigned int tesselationCount /*= 4*/, Rgba8 const& color /*= Rgba8::WHITE*/, 
    Vec2 const& uvMins /*= Vec2::ZERO*/, Vec2 const& uvMaxs /*= Vec2::ONE*/)
{
    std::vector<Vertex_PCUTBN> sphereVerts;
    std::vector<unsigned int> sphereInds;

    Vec3 halfCube(1.f, 1.f, 1.f);
    AABB3 cube(center - halfCube, center + halfCube);
    AppendIndexedVertexesForAABB3D(sphereVerts, sphereInds, cube, color, uvMins, uvMaxs);

    for (unsigned int i = 0; i < tesselationCount; i++) {
        Tesselate(sphereVerts, sphereInds);
    }

    //stretch to sphere
    for (size_t i = 0; i < sphereVerts.size(); i++) {
        Vertex_PCUTBN& vert = sphereVerts[i];
        Vec3 centerToP = (vert.position - center).GetNormalized();
        vert.position = centerToP*radius + center;
        vert.tangent = Vec4(Vec3(centerToP.z, 0.f, -centerToP.x).GetNormalized(), 1.f);
        vert.normal = centerToP;
    }

    unsigned int vertStartIdx = (unsigned int)vertexes.size();
    for (size_t i = 0; i < sphereInds.size(); i++) {
        sphereInds[i] += vertStartIdx;
    }

    vertexes.insert(vertexes.end(), sphereVerts.begin(), sphereVerts.end());
    indices.insert(indices.end(), sphereInds.begin(), sphereInds.end());
}


//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForIcoSphere(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices, 
    Vec3 const& center, float radius, unsigned int subdivision /*= 4*/, Rgba8 const& color /*= Rgba8::WHITE*/)
{
    if (subdivision == 0) {
        return;
    }
    
    std::vector<Vertex_PCU> sphereVerts;
    std::vector<unsigned int> sphereInds;
    float goldenLength = (1.f + std::sqrtf(5.f)) * .5f;

    //Triangle 1
    sphereVerts.push_back(Vertex_PCU(Vec3(-1.f, goldenLength, 0.f), color, Vec2::ZERO));
    sphereVerts.push_back(Vertex_PCU(Vec3(1.f, goldenLength, 0.f), color, Vec2::ZERO));
    sphereVerts.push_back(Vertex_PCU(Vec3(1.f, -goldenLength, 0.f), color, Vec2::ZERO));
    sphereVerts.push_back(Vertex_PCU(Vec3(-1.f, -goldenLength, 0.f), color, Vec2::ZERO));
    //triangle 2
    sphereVerts.push_back(Vertex_PCU(Vec3(0.f, 1.f, -goldenLength), color, Vec2::ZERO));
    sphereVerts.push_back(Vertex_PCU(Vec3(0.f, 1.f, goldenLength), color, Vec2::ZERO));
    sphereVerts.push_back(Vertex_PCU(Vec3(0.f, -1.f, goldenLength), color, Vec2::ZERO));
    sphereVerts.push_back(Vertex_PCU(Vec3(0.f, -1.f, -goldenLength), color, Vec2::ZERO));
    // 3
    sphereVerts.push_back(Vertex_PCU(Vec3(-goldenLength, 0.f, -1.f), color, Vec2::ZERO));
    sphereVerts.push_back(Vertex_PCU(Vec3(goldenLength, 0.f, -1.f), color, Vec2::ZERO));
    sphereVerts.push_back(Vertex_PCU(Vec3(goldenLength, 0.f, 1.f), color, Vec2::ZERO));
    sphereVerts.push_back(Vertex_PCU(Vec3(-goldenLength, 0.f, 1.f), color, Vec2::ZERO));

    //Face 1
    sphereInds.push_back(0);
    sphereInds.push_back(5);
    sphereInds.push_back(1);
    //2                  
    sphereInds.push_back(0);
    sphereInds.push_back(1);
    sphereInds.push_back(4);
    //3                  
    sphereInds.push_back(0);
    sphereInds.push_back(4);
    sphereInds.push_back(8);
    //4                  
    sphereInds.push_back(0);
    sphereInds.push_back(8);
    sphereInds.push_back(11);
    //5                  
    sphereInds.push_back(0);
    sphereInds.push_back(11);
    sphereInds.push_back(5);
    //6                  
    sphereInds.push_back(1);
    sphereInds.push_back(5);
    sphereInds.push_back(10);
    //7                  
    sphereInds.push_back(1);
    sphereInds.push_back(10);
    sphereInds.push_back(9);
    //8                  
    sphereInds.push_back(1);
    sphereInds.push_back(9);
    sphereInds.push_back(4);
    //9                  
    sphereInds.push_back(3);
    sphereInds.push_back(6);
    sphereInds.push_back(11);
    //10                 
    sphereInds.push_back(3);
    sphereInds.push_back(2);
    sphereInds.push_back(6);
    //11                 
    sphereInds.push_back(3);
    sphereInds.push_back(7);
    sphereInds.push_back(2);
    //12                 
    sphereInds.push_back(3);
    sphereInds.push_back(8);
    sphereInds.push_back(7);
    //13                 
    sphereInds.push_back(3);
    sphereInds.push_back(11);
    sphereInds.push_back(8);
    //14                 
    sphereInds.push_back(2);
    sphereInds.push_back(10);
    sphereInds.push_back(6);
    //15                 
    sphereInds.push_back(2);
    sphereInds.push_back(9);
    sphereInds.push_back(10);
    //16                 
    sphereInds.push_back(2);
    sphereInds.push_back(7);
    sphereInds.push_back(9);
    //17                 
    sphereInds.push_back(4);
    sphereInds.push_back(7);
    sphereInds.push_back(8);
    //18                 
    sphereInds.push_back(4);
    sphereInds.push_back(9);
    sphereInds.push_back(7);
    //19                 
    sphereInds.push_back(11);
    sphereInds.push_back(6);
    sphereInds.push_back(5);
    //20                 
    sphereInds.push_back(6);
    sphereInds.push_back(10);
    sphereInds.push_back(5);

    //subdivide
    for (unsigned int i = 1; i < subdivision; i++) {
        Tesselate(sphereVerts, sphereInds);
    }

    //push
    for (size_t i = 0; i < sphereVerts.size(); i++) {
        Vec3 centerToP = sphereVerts[i].position;
        centerToP.SetLength(radius);
        sphereVerts[i].position = centerToP + center;
    }

    //update indices
    unsigned int vertStartIdx = (unsigned int)vertexes.size();
    for (size_t i = 0; i < sphereInds.size(); i++) {
        sphereInds[i] += vertStartIdx;
    }

    vertexes.insert(vertexes.end(), sphereVerts.begin(), sphereVerts.end());
    indices.insert(indices.end(), sphereInds.begin(), sphereInds.end());
}

//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForZUpCylinder(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices, 
    Vec3 const& localCenter, float bottomRadius, float upRadius, float length, unsigned int verticalCuts /*= CIRCLE_FRAGMENT_NUM*/, 
    Rgba8 const& color /*= Rgba8::WHITE*/)
{
    if (verticalCuts < 3) {
        return;
    }

    float halfLength = length * .5f;
    Vec3 bottomCenter = localCenter + Vec3(0.f,0.f,-halfLength);
    Vec3 upCenter = localCenter + Vec3(0.f, 0.f, halfLength);
    
    float unitDegrees = 360.f / (float)verticalCuts;
    //bottom
    unsigned int bottomStartIdx = (unsigned int)vertexes.size();
    vertexes.push_back(Vertex_PCU(bottomCenter, color));
    for (unsigned int i = 0; i < verticalCuts; i++) {
        Vec2 discPos = Vec2::MakeFromPolarDegrees(unitDegrees * (float)i, bottomRadius);
        Vec3 pos = bottomCenter + Vec3(discPos.x,discPos.y,0.f);
        vertexes.push_back(Vertex_PCU(pos, color));
    }
    //up
    unsigned int upStartIdx = (unsigned int)vertexes.size();
    vertexes.push_back(Vertex_PCU(upCenter, color));
    for (unsigned int i = 0; i < verticalCuts; i++) {
        Vec2 discPos = Vec2::MakeFromPolarDegrees(unitDegrees * (float)i, upRadius);
        Vec3 pos = upCenter + Vec3(discPos.x, discPos.y, 0.f);
        vertexes.push_back(Vertex_PCU(pos, color));
    }

    //bottom
    for (unsigned int i = 0; i < verticalCuts; i++) {
        indices.push_back(bottomStartIdx);
        indices.push_back(bottomStartIdx + i + 1);
        if (i == verticalCuts - 1) {
            indices.push_back(bottomStartIdx + 1);
        }
        else {
            indices.push_back(bottomStartIdx + i + 2);
        }
    }
    //up
    for (unsigned int i = upStartIdx+verticalCuts; i > upStartIdx; i--) {
        indices.push_back(upStartIdx);
        indices.push_back(i);
        if (i == upStartIdx+1) {
            indices.push_back(upStartIdx + verticalCuts);
        }
        else {
            indices.push_back(i-1);
        }
    }
    //side
    for (unsigned int i = 1; i < verticalCuts+1; i++) {
        unsigned int bottomLeft = bottomStartIdx + i;
        unsigned int bottomRight = i == 1 ? upStartIdx - 1 : bottomStartIdx + i - 1;
        unsigned int topRight = i == 1 ? upStartIdx + verticalCuts : upStartIdx + i - 1;
        unsigned int topLeft = upStartIdx + i;
        indices.push_back(bottomLeft);
        indices.push_back(bottomRight);
        indices.push_back(topRight);
        indices.push_back(bottomLeft);
        indices.push_back(topRight);
        indices.push_back(topLeft);
    }
}

//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForCylinder(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices, Vec3 const& startCenter, Vec3 const& endCenter, float radius, unsigned int verticalCuts /*= CIRCLE_FRAGMENT_NUM*/, Rgba8 const& color /*= Rgba8::WHITE*/)
{
    std::vector<Vertex_PCU> verts;
    std::vector<unsigned int> inds;

    AppendIndexedVertexesForZUpCylinder(verts, inds, Vec3::ZERO, radius, radius, (endCenter-startCenter).GetLength(), verticalCuts, color);

    Mat44 lookat = MatrixLookAt(startCenter, endCenter);
    lookat.SetBasisVectors3D(lookat.GetIBasis3D(), lookat.GetJBasis3D(), -lookat.GetKBasis3D(), (startCenter + endCenter) * .5f);
    TransformVertexArray((int)verts.size(), &verts[0], lookat);

    unsigned int vertsStartIdx = (unsigned int)vertexes.size();
    vertexes.insert(vertexes.end(), verts.begin(), verts.end());
    for (size_t i = 0; i < inds.size(); i++) {
        indices.push_back(vertsStartIdx + inds[i]);
    }
}

//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForCone(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    Vec3 const& bottomCenter, float radius, float height, Vec3 const& upDirection, 
    unsigned int verticalCuts /*= CIRCLE_FRAGMENT_NUM*/, Rgba8 const& color /*= Rgba8::WHITE*/)
{
    std::vector<Vertex_PCU> verts;
    std::vector<unsigned int> inds;

    AppendIndexedVertexesForZUpCylinder(verts, inds, Vec3(0.f, 0.f, height * .5f), radius, 0.f, height, verticalCuts, color);

    Mat44 lookat = MatrixLookAt(bottomCenter, bottomCenter+upDirection*height);
    lookat.SetBasisVectors3D(lookat.GetIBasis3D(), lookat.GetJBasis3D(), -lookat.GetKBasis3D(), bottomCenter);
    TransformVertexArray((int)verts.size(), &verts[0], lookat);

    unsigned int vertsStartIdx = (unsigned int)vertexes.size();
    vertexes.insert(vertexes.end(), verts.begin(), verts.end());
    for (size_t i = 0; i < inds.size(); i++) {
        indices.push_back(vertsStartIdx + inds[i]);
    }
}

//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForArrow(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices, 
    Vec3 const& start, Vec3 const& end, float baseWidth, float maxWidth, 
    unsigned int verticalCuts /*= CIRCLE_FRAGMENT_NUM*/, Rgba8 const& color /*= Rgba8::WHITE*/)
{
    std::vector<Vertex_PCU> verts;
    std::vector<unsigned int> inds;

    Vec3 middle = (start + end) * .5f;
    float length = (middle - start).GetLength();
    Vec3 center0 = Vec3(0.f, 0.f, length * .5f);
    Vec3 center1 = Vec3(0.f, 0.f, length * 1.5f);
    AppendIndexedVertexesForZUpCylinder(verts, inds, center0, baseWidth, baseWidth, length, verticalCuts, color);
    AppendIndexedVertexesForZUpCylinder(verts, inds, center1, maxWidth, 0.f, length, verticalCuts, color);

    Mat44 lookat = MatrixLookAt(start, end);
    lookat.SetBasisVectors3D(lookat.GetIBasis3D(), lookat.GetJBasis3D(), -lookat.GetKBasis3D(), start);
    TransformVertexArray((int)verts.size(), &verts[0], lookat);

    unsigned int vertsStartIdx = (unsigned int)vertexes.size();
    vertexes.insert(vertexes.end(), verts.begin(), verts.end());
    for (size_t i = 0; i < inds.size(); i++) {
        indices.push_back(vertsStartIdx + inds[i]);
    }
}

//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForTriangle2D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices, 
    Vec3 const& p0, Vec3 const& p1, Vec3 const& p2, Rgba8 const& color /*= Rgba8::WHITE*/)
{
    unsigned int vertsStartIdx = (unsigned int)vertexes.size();
    vertexes.push_back(Vertex_PCU(p0, color));
    vertexes.push_back(Vertex_PCU(p1, color));
    vertexes.push_back(Vertex_PCU(p2, color));
    indices.push_back(vertsStartIdx);
    indices.push_back(vertsStartIdx + 1);
    indices.push_back(vertsStartIdx + 2);
}

//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForDisc2D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices,
    Vec2 const& center, float radius, Rgba8 const& color /*= Rgba8::WHITE*/, unsigned int fragmentNum /*= 32*/)
{
    unsigned int vertStartIdx = (unsigned int)vertexes.size();
    vertexes.push_back(Vertex_PCU(center, color));
    float unitDegrees = 360.f / (float)fragmentNum;
    for (unsigned int vertID = 0; vertID < fragmentNum; vertID++)
    {
        Vec2 newPos = center + Vec2::MakeFromPolarDegrees(unitDegrees * (float)vertID, radius);
        vertexes.push_back(Vertex_PCU(newPos, color));
    }

    for (unsigned int i = 0; i < fragmentNum; i++) {
        indices.push_back(vertStartIdx);
        indices.push_back(vertStartIdx + i + 1);
        if (i == fragmentNum - 1) {
            indices.push_back(vertStartIdx+1);
        }
        else {
            indices.push_back(vertStartIdx + i + 2);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void AppendIndexedVertexesForLine2D(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices, 
    Vec2 const& start, Vec2 const& end, float width, Rgba8 const& color /*= Rgba8::WHITE*/)
{
    Vec2 forward = (end - start).GetNormalized();
    Vec2 verticle = forward.GetRotated90Degrees();

    float halfWidth = width * .5f;
    unsigned int vertsStartIdx = (unsigned int)vertexes.size();
    vertexes.push_back(Vertex_PCU(start - verticle * halfWidth, color));
    vertexes.push_back(Vertex_PCU(start + verticle * halfWidth, color));
    vertexes.push_back(Vertex_PCU(end - verticle * halfWidth, color));
    vertexes.push_back(Vertex_PCU(end + verticle * halfWidth, color));

    indices.push_back(vertsStartIdx);
    indices.push_back(vertsStartIdx + 2);
    indices.push_back(vertsStartIdx + 3);
    indices.push_back(vertsStartIdx);
    indices.push_back(vertsStartIdx + 3);
    indices.push_back(vertsStartIdx + 1);
}

//////////////////////////////////////////////////////////////////////////
void Tesselate(std::vector<Vertex_PCU>& vertexes, std::vector<unsigned int>& indices)
{
    std::vector<unsigned int> originIndices = indices;
    indices.clear();

    size_t vertsNum = vertexes.size();
    size_t indicesNum = originIndices.size();

    size_t* indexes = new size_t[vertsNum * vertsNum];
    for (int i = 0; i < vertsNum * vertsNum; i++) {
        indexes[i] = SIZE_MAX;
    }

    for (size_t i = 0; 3 * i < indicesNum; i++) {
        size_t start = 3 * i;
        size_t startVertIdx = vertexes.size();

        unsigned int first = originIndices[start];
        unsigned int second = originIndices[start + 1];
        unsigned int third = originIndices[start + 2];

        Vertex_PCU firstVP = vertexes[first];
        Vertex_PCU secondVP = vertexes[second];
        Vertex_PCU thirdVP = vertexes[third];

        unsigned int fsIdx = first * (unsigned int)vertsNum + second;
        size_t fsMiddle = indexes[fsIdx];
        if (fsMiddle == SIZE_MAX) {
            vertexes.push_back(GetMiddleVertex(firstVP, secondVP));
            fsMiddle = indexes[fsIdx] = indexes[second * vertsNum + first] = startVertIdx;
            startVertIdx++;
        }

        unsigned int stIdx = second * (unsigned int)vertsNum + third;
        size_t stMiddle = indexes[stIdx];
        if (stMiddle == SIZE_MAX) {
            vertexes.push_back(GetMiddleVertex(secondVP, thirdVP));
            stMiddle = indexes[stIdx] = indexes[third * vertsNum + second] = startVertIdx;
            startVertIdx++;
        }

        unsigned int ftIdx = first * (unsigned int)vertsNum + third;
        size_t ftMiddle = indexes[ftIdx];
        if (ftMiddle == SIZE_MAX) {
            vertexes.push_back(GetMiddleVertex(firstVP, thirdVP));
            ftMiddle = indexes[ftIdx] = indexes[third * vertsNum + first] = startVertIdx;
            startVertIdx++;
        }

        indices.push_back(first);
        indices.push_back((unsigned int)fsMiddle);
        indices.push_back((unsigned int)ftMiddle);

        indices.push_back((unsigned int)fsMiddle);
        indices.push_back(second);
        indices.push_back((unsigned int)stMiddle);

        indices.push_back((unsigned int)ftMiddle);
        indices.push_back((unsigned int)stMiddle);
        indices.push_back(third);

        indices.push_back((unsigned int)fsMiddle);
        indices.push_back((unsigned int)stMiddle);
        indices.push_back((unsigned int)ftMiddle);
    }

    delete[] indexes;
}


//////////////////////////////////////////////////////////////////////////
void Tesselate(std::vector<Vertex_PCUTBN>& vertexes, std::vector<unsigned int>& indices)
{
    std::vector<unsigned int> originIndices = indices;
    indices.clear();

    size_t vertsNum = vertexes.size();
    size_t indicesNum = originIndices.size();

    size_t* indexes = new size_t[vertsNum * vertsNum];
    for (int i = 0; i < vertsNum * vertsNum; i++) {
        indexes[i] = SIZE_MAX;
    }

    for (size_t i = 0; 3 * i < indicesNum; i++) {
        size_t start = 3 * i;
        size_t startVertIdx = vertexes.size();

        unsigned int first = originIndices[start];
        unsigned int second = originIndices[start + 1];
        unsigned int third = originIndices[start + 2];

        Vertex_PCUTBN firstVP = vertexes[first];
        Vertex_PCUTBN secondVP = vertexes[second];
        Vertex_PCUTBN thirdVP = vertexes[third];

        unsigned int fsIdx = first * (unsigned int)vertsNum + second;
        size_t fsMiddle = indexes[fsIdx];
        if (fsMiddle == SIZE_MAX) {
            vertexes.push_back(GetMiddleVertex(firstVP, secondVP));
            fsMiddle = indexes[fsIdx] = indexes[second * vertsNum + first] = startVertIdx;
            startVertIdx++;
        }

        unsigned int stIdx = second * (unsigned int)vertsNum + third;
        size_t stMiddle = indexes[stIdx];
        if (stMiddle == SIZE_MAX) {
            vertexes.push_back(GetMiddleVertex(secondVP, thirdVP));
            stMiddle = indexes[stIdx] = indexes[third * vertsNum + second] = startVertIdx;
            startVertIdx++;
        }

        unsigned int ftIdx = first * (unsigned int)vertsNum + third;
        size_t ftMiddle = indexes[ftIdx];
        if (ftMiddle == SIZE_MAX) {
            vertexes.push_back(GetMiddleVertex(firstVP, thirdVP));
            ftMiddle = indexes[ftIdx] = indexes[third * vertsNum + first] = startVertIdx;
            startVertIdx++;
        }

        indices.push_back(first);
        indices.push_back((unsigned int)fsMiddle);
        indices.push_back((unsigned int)ftMiddle);

        indices.push_back((unsigned int)fsMiddle);
        indices.push_back(second);
        indices.push_back((unsigned int)stMiddle);

        indices.push_back((unsigned int)ftMiddle);
        indices.push_back((unsigned int)stMiddle);
        indices.push_back(third);

        indices.push_back((unsigned int)fsMiddle);
        indices.push_back((unsigned int)stMiddle);
        indices.push_back((unsigned int)ftMiddle);
    }

    delete[] indexes;
}
