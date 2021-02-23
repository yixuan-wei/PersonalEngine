#include "Engine/Core/OBJUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "ThirdParty/mikktspace/mikktspace.h"

enum OBJReadStage
{
    READ_INVALID,
    READ_VERT,
    READ_NORMAL,
    READ_TANGENT,
    READ_FACE
};

//////////////////////////////////////////////////////////////////////////
static OBJReadStage GetOBJReadStageFromString(std::string const& s)
{
    if (s.compare("v")==0) {
        return READ_VERT;
    }
    else if (s.compare("vn")==0) {
        return READ_NORMAL;
    }
    else if (s.compare("vt") == 0) {
        return READ_TANGENT;
    }
    else if (s.compare("f") == 0) {
        return READ_FACE;
    }

    return READ_INVALID;
}

//////////////////////////////////////////////////////////////////////////
static int GetNumFaces(SMikkTSpaceContext const* context)
{
    std::vector<Vertex_PCUTBN>& verts = *(std::vector<Vertex_PCUTBN>*)(context->m_pUserData);
    return (int)verts.size() / 3;
}

//////////////////////////////////////////////////////////////////////////
static int GetNumVerticesForFace(SMikkTSpaceContext const* pContext, const int iFace)
{
    UNUSED(pContext);
    UNUSED(iFace);
    return 3;
}

//////////////////////////////////////////////////////////////////////////
static void GetPositionForFaceVert(SMikkTSpaceContext const* pContext, float fvPosOut[], const int iFace, const int iVert)
{
    std::vector<Vertex_PCUTBN>& verts = *(std::vector<Vertex_PCUTBN>*)(pContext->m_pUserData);
    int idxInVerts = iFace * 3 + iVert;

    Vec3 pos = verts[idxInVerts].position;
    fvPosOut[0] = pos.x;
    fvPosOut[1] = pos.y;
    fvPosOut[2] = pos.z;
}

//////////////////////////////////////////////////////////////////////////
static void GetNormalForFaceVert(SMikkTSpaceContext const* pContext, float fvNormalOut[], const int iFace, const int iVert)
{
    std::vector<Vertex_PCUTBN>& verts = *(std::vector<Vertex_PCUTBN>*)(pContext->m_pUserData);
    int idxInVerts = iFace * 3 + iVert;

    Vec3 normal = verts[idxInVerts].normal;
    fvNormalOut[0] = normal.x;
    fvNormalOut[1] = normal.y;
    fvNormalOut[2] = normal.z;
}

//////////////////////////////////////////////////////////////////////////
static void GetUVForFaceVert(SMikkTSpaceContext const* pContext, float fvTexOut[], const int iFace, const int iVert)
{
    std::vector<Vertex_PCUTBN>& verts = *(std::vector<Vertex_PCUTBN>*)(pContext->m_pUserData);
    int idxInVerts = iFace * 3 + iVert;

    Vec2 uvCoords = verts[idxInVerts].uvTexCoords;
    fvTexOut[0] = uvCoords.x;
    fvTexOut[1] = uvCoords.y;
}

//////////////////////////////////////////////////////////////////////////
static void SetTangent(const SMikkTSpaceContext* pContext,
    const float fvTangent[],
    const float fSign,
    const int iFace, const int iVert)
{
    std::vector<Vertex_PCUTBN>& vertices = *(std::vector<Vertex_PCUTBN>*)(pContext->m_pUserData);
    int indexInVertexArray = iFace * 3 + iVert;

    vertices[indexInVertexArray].tangent = Vec4(fvTangent[0], fvTangent[1], fvTangent[2], fSign);
}

//////////////////////////////////////////////////////////////////////////
void MeshGenerateTangents(std::vector<Vertex_PCUTBN>& vertices)
{
    SMikkTSpaceInterface interface;
    //set info
    interface.m_getNumFaces = GetNumFaces;
    interface.m_getNumVerticesOfFace = GetNumVerticesForFace;
    interface.m_getPosition = GetPositionForFaceVert;
    interface.m_getNormal = GetNormalForFaceVert;
    interface.m_getTexCoord = GetUVForFaceVert;
    //get info
    interface.m_setTSpaceBasic = SetTangent;    
    interface.m_setTSpace = nullptr;

    SMikkTSpaceContext context;
    context.m_pInterface = &interface;
    context.m_pUserData = &vertices;

    // RUN 
    genTangSpaceDefault(&context);
}

//////////////////////////////////////////////////////////////////////////
void CleanVertexesForIndexedVertexArray(std::vector<Vertex_PCUTBN> const& rawVerts, std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indices)
{
    for (size_t i = 0; i < rawVerts.size(); i++) {
        Vertex_PCUTBN vert = rawVerts[i];

        bool foundExisted = false;
        for (size_t j = 0; j < verts.size(); j++) {
            Vertex_PCUTBN& existedV = verts[j];

            if (vert.position == existedV.position && vert.tangent == existedV.tangent && vert.normal == existedV.normal
                && vert.tint == existedV.tint && vert.uvTexCoords == existedV.uvTexCoords) {
                indices.push_back((unsigned int)j);
                foundExisted = true;
                break;
            }
        }
        if (!foundExisted) {
            indices.push_back((unsigned int)verts.size());
            verts.push_back(vert);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
static void AppendToVertexArrayForOBJFaceReading(std::vector<Vertex_PCUTBN>& verts,
    std::vector<Vec3> const& pos, std::vector<Vec3>const& normals, std::vector<Vec2> const& texCoords,
    Strings const& trunks)
{
    std::vector<Vertex_PCUTBN> newVerts;
    for (std::string trunk : trunks) {
        Strings ints = SplitStringOnDelimiter(Trim(trunk), '/');
        GUARANTEE_OR_DIE(ints.size() == 2 || ints.size() == 3, Stringf("OBJ format wrong: face %s", trunk.c_str()));
        int p=0, n=0, t=0;
        Vec3 normal(0.f, 0.f, 1.f);
        Vec3 tangent(1.f, 0.f, 0.f);
        Translate(ints[0], &p);
        Translate(ints[1], &t);
        if (ints.size() == 3) {
            Translate(ints[2], &n);
            normal = normals[n - 1];
        }        
        newVerts.push_back(Vertex_PCUTBN(pos[p - 1], Rgba8::WHITE, texCoords[t - 1], Vec4(tangent,1.f), normal));
    }

    if (trunks.size() == 3) {
        verts.push_back(newVerts[0]);
        verts.push_back(newVerts[1]);
        verts.push_back(newVerts[2]);
    }
    else if (trunks.size() == 4) {
        verts.push_back(newVerts[0]);
        verts.push_back(newVerts[1]);
        verts.push_back(newVerts[2]);

        verts.push_back(newVerts[0]);
        verts.push_back(newVerts[2]);
        verts.push_back(newVerts[3]);
    }
}

//////////////////////////////////////////////////////////////////////////
void LoadOBJToVertexArray(std::vector<Vertex_PCUTBN>& verts, char const* filename, obj_import_options const& options)
{
    UNUSED(options);

    std::vector<Vertex_PCUTBN> rawVerts;
    Strings lines = FileReadLines(filename);
    OBJReadStage readStage = OBJReadStage::READ_VERT;
    std::vector<Vec3> vertexes;
    std::vector<Vec3> normals;
    std::vector<Vec2> texCoords;

    for (std::string line : lines) {
        std::string cleanLine = Trim(line);
        if (cleanLine.empty() || cleanLine[0] == '#') {
            continue;
        }

        Strings trunks = SplitStringOnDelimiter(cleanLine, ' ');
        ClearEmptyStringInStrings(trunks);

        OBJReadStage curStage = GetOBJReadStageFromString(trunks[0]);
        if (curStage == READ_INVALID) {
            g_theConsole->PrintString(Rgba8::MAGENTA, Stringf("OBJ not support %s", trunks[0].c_str()));
            continue;
        }
        GUARANTEE_OR_DIE(curStage >= readStage, "OBJ format wrong: stage order");
        readStage = curStage;
        trunks.erase(trunks.begin());

        if (curStage < READ_TANGENT) {
            //vec3 format assumed
            GUARANTEE_OR_DIE(trunks.size() == 3, Stringf("OBJ format wrong: %s", cleanLine.c_str()));
            Vec3 values;
            Translate(trunks[0], &values.x);
            Translate(trunks[1], &values.y);
            Translate(trunks[2], &values.z);
            switch (curStage)
            {
            case READ_VERT:     vertexes.push_back(values); break;
            case READ_NORMAL:   normals.push_back(values);  break;
            }
        }
        else if (curStage == READ_TANGENT) {
            GUARANTEE_OR_DIE(trunks.size() == 3 || trunks.size() == 2, Stringf("OBJ format wrong: %s", cleanLine.c_str()));
            Vec3 values;
            Translate(trunks[0], &values.x);
            Translate(trunks[1], &values.y);
            if (trunks.size() == 3) {
                Translate(trunks[2], &values.z);
                GUARANTEE_OR_DIE(values.z == 0.f, Stringf("OBJ format wrong: %s", cleanLine.c_str()));
            }
            texCoords.push_back(values.GetXY());
        }
        else {
            GUARANTEE_OR_DIE(trunks.size() == 3 || trunks.size() == 4, Stringf("OBJ format wrong: %s", cleanLine.c_str()));
            AppendToVertexArrayForOBJFaceReading(rawVerts, vertexes, normals, texCoords, trunks);
        }
    }

    if (options.invertVCoord) {
        MeshInvertV(rawVerts);
    }
    if (options.generateNormals && normals.size()<1) {
        MeshCalculateNormal(rawVerts);
    }
    if (options.smoothNormals) {
        MeshSmoothNormal(rawVerts);
    }
    if (options.generateTangents) {
        MeshGenerateTangents(rawVerts);
    }
    TransformVertexArray((int)rawVerts.size(), &rawVerts[0], options.transform);
    if (options.invertWindingOrder) {
        MeshInvertWindingOrder(rawVerts);
    }
    
    verts.insert(verts.end(),rawVerts.begin(),rawVerts.end());
}

//////////////////////////////////////////////////////////////////////////
void MeshInvertV(std::vector<Vertex_PCUTBN>& verts)
{
    for (size_t i = 0; i < verts.size(); i++) {
        Vertex_PCUTBN& v = verts[i];
        v.uvTexCoords.y = -v.uvTexCoords.y;
    }
}

//////////////////////////////////////////////////////////////////////////
void MeshCalculateNormal(std::vector<Vertex_PCUTBN>& verts)
{
    for (size_t i = 0; 3 * i < verts.size(); i++) {
        Vertex_PCUTBN& v0 = verts[3 * i];
        Vertex_PCUTBN& v1 = verts[3 * i + 1];
        Vertex_PCUTBN& v2 = verts[3 * i + 2];
        Vec3 normal = CrossProduct3D(v1.position - v0.position, v2.position - v0.position);
        v0.normal = normal;
        v1.normal = normal;
        v2.normal = normal;
    }
}

//////////////////////////////////////////////////////////////////////////
void MeshSmoothNormal(std::vector<Vertex_PCUTBN>& verts)
{
    std::vector<bool> calculated;
    for (size_t i = 0; i < verts.size(); i++) {
        calculated.push_back(false);
    }

    for (size_t i = 0; i < verts.size(); i++) {
        if (calculated[i]) {
            continue;
        }

        Vertex_PCUTBN& vert = verts[i];
        calculated[i] = true;
        std::vector<Vec3> normals;
        std::vector<float> areas;
        std::vector<size_t> indexes;
        normals.push_back(vert.normal);
        indexes.push_back(i);
        Vec3 p0, p1, p2;
        if (i % 3 == 0) {
            p0 = vert.position;
            p1 = verts[i + 1].position;
            p2 = verts[i + 2].position;
        }
        else if (i % 3 == 1) {
            p0 = verts[i - 1].position;
            p1 = vert.position;
            p2 = verts[i + 1].position;
        }
        else {
            p0 = verts[i - 2].position;
            p1 = verts[i - 1].position;
            p2 = vert.position;
        }
        areas.push_back(CrossProduct3D(p1 - p0, p2 - p0).GetLength());

        for (size_t j = i + 1; j < verts.size(); j++) {
            Vertex_PCUTBN& v1 = verts[j];
            if (v1.position == vert.position && v1.uvTexCoords ==vert.uvTexCoords) {
                calculated[j] = true;
                normals.push_back(v1.normal);
                indexes.push_back(j);
                Vec3 tp0, tp1, tp2;
                if (j % 3 == 0) {
                    tp0 = v1.position;
                    tp1 = verts[j + 1].position;
                    tp2 = verts[j + 2].position;
                }
                else if (j % 3 == 1) {
                    tp0 = verts[j - 1].position;
                    tp1 = v1.position;
                    tp2 = verts[j + 1].position;
                }
                else {
                    tp0 = verts[j - 2].position;
                    tp1 = verts[j - 1].position;
                    tp2 = v1.position;
                }
                areas.push_back(CrossProduct3D(tp1 - tp0, tp2 - tp0).GetLength());
            }
        }

        if (normals.size() < 2) {
            continue;
        }

        Vec3 normalSum;
        float areaSum = 0.f;
        for (size_t k = 0; k < areas.size(); k++) {
            normalSum += normals[k] * areas[k];
            areaSum += areas[k];
        }
        areaSum = areaSum == 0.f ? 1.f : areaSum;
        Vec3 finalNormal = normalSum / areaSum;
        finalNormal.Normalize();
        for (size_t k = 0; k < indexes.size(); k++) {
            verts[indexes[k]].normal = finalNormal;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void MeshInvertWindingOrder(std::vector<Vertex_PCUTBN>& verts)
{
    for (size_t i = 0; 3 * i < verts.size(); i++) {
        Vertex_PCUTBN temp = verts[3 * i + 2];
        verts[3 * i + 2] = verts[3 * i + 1];
        verts[3 * i + 1] = temp;
    }
}

//////////////////////////////////////////////////////////////////////////
void LoadOBJToIndexedVertexArray(std::vector<Vertex_PCUTBN>& verts, std::vector<unsigned int>& indices,
    char const* filename, obj_import_options const& options)
{
    std::vector<Vertex_PCUTBN> rawVerts;
    LoadOBJToVertexArray(rawVerts, filename, options);

    CleanVertexesForIndexedVertexArray(rawVerts, verts, indices);
    verts.insert(verts.end(), rawVerts.begin(), rawVerts.end());
}
