#include "Engine/Core/Vertex_PCUTBN.hpp"
#include "Engine/Renderer/buffer_attribute_t.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MatrixUtils.hpp"

buffer_attribute_t const Vertex_PCUTBN::LAYOUT[] =
{
    buffer_attribute_t("POSITION",  eBufferFormatType::BUFFER_FORMAT_VEC3,              offsetof(Vertex_PCUTBN, position)),
    buffer_attribute_t("COLOR",     eBufferFormatType::BUFFER_FORMAT_R8G8B8A8_UNORM,    offsetof(Vertex_PCUTBN, tint)),
    buffer_attribute_t("TEXCOORD",  eBufferFormatType::BUFFER_FORMAT_VEC2,              offsetof(Vertex_PCUTBN, uvTexCoords)),
    buffer_attribute_t("TANGENT",   eBufferFormatType::BUFFER_FORMAT_VEC4,              offsetof(Vertex_PCUTBN, tangent)),
    buffer_attribute_t("NORMAL",    eBufferFormatType::BUFFER_FORMAT_VEC3,              offsetof(Vertex_PCUTBN, normal)),
    buffer_attribute_t()    //terminator element
};

//////////////////////////////////////////////////////////////////////////
Vertex_PCUTBN::Vertex_PCUTBN(Vec3 const& pos, Rgba8 const& color, Vec2 const& uv /*= Vec2::ZERO*/, 
    Vec4 const& t /*= Vec4(1.f, 0.f, 0.f, 1.f)*/, Vec3 const& n /*= Vec3(0.f, 0.f, 1.f)*/)
    : position(pos)
    , tint(color)
    , uvTexCoords(uv)
    , tangent(t)
    , normal(n)
{
}

//////////////////////////////////////////////////////////////////////////
Vertex_PCUTBN GetMiddleVertex(Vertex_PCUTBN const& VPA, Vertex_PCUTBN const& VPB)
{
    Vec3 position = (VPA.position + VPB.position) * .5f;
    Vec2 uv = (VPA.uvTexCoords + VPB.uvTexCoords) * .5f;
    Rgba8 color = Lerp(VPA.tint, VPB.tint);
    Vec3 tangent = (VPA.tangent.GetXYZ() + VPB.tangent.GetXYZ()) * .5f;
    tangent.Normalize();
    Vec3 normal = (VPA.normal + VPB.normal) * .5f;
    normal.Normalize();
    return Vertex_PCUTBN(position, color, uv, Vec4(tangent, VPA.tangent.w*VPB.tangent.w), normal);
}

//////////////////////////////////////////////////////////////////////////
void TransformVertexArray(int vertexesNumber, Vertex_PCUTBN* vertexes, Mat44 const& transformMat)
{
    Mat44 normalMat = transformMat;
    normalMat.SetTranslation3D(Vec3::ZERO);
    normalMat = MatrixTranspose(MatrixInverse(normalMat));
    for (int i = 0; i < vertexesNumber; i++) {
        Vertex_PCUTBN& vert = vertexes[i];
        vert.position = transformMat.TransformPosition3D(vert.position);
        vert.normal = normalMat.TransformVector3D(vert.normal).GetNormalized();
        Vec3 tangentDir = normalMat.TransformVector3D(vert.tangent.GetXYZ()).GetNormalized();
        vert.tangent = Vec4(tangentDir, vert.tangent.w);
    }
}

//////////////////////////////////////////////////////////////////////////
void TransformVertexArray(int vertexesNumber, Vertex_PCUTBN* vertexes, float uniformScale, float rotationAboutZDegrees, Vec3 const& translation)
{
    Mat44 transformMat = Mat44::CreateZRotationDegrees(rotationAboutZDegrees);
    transformMat.ScaleUniform3D(uniformScale);
    transformMat.SetTranslation3D(translation);

    Mat44 normalMat = transformMat;
    normalMat.SetTranslation3D(Vec3::ZERO);
    normalMat = MatrixTranspose(MatrixInverse(normalMat));

    for (int i = 0; i < vertexesNumber; i++) {
        Vertex_PCUTBN& vert = vertexes[i];
        vert.position = transformMat.TransformPosition3D(vert.position);
        vert.normal = normalMat.TransformVector3D(vert.normal).GetNormalized();
        Vec3 tangentDir = normalMat.TransformVector3D(vert.tangent.GetXYZ()).GetNormalized();
        vert.tangent = Vec4(tangentDir, vert.tangent.w);
    }
}

//////////////////////////////////////////////////////////////////////////
void TransformVertexArray(int vertexesNumber, Vertex_PCUTBN* vertexes, float uniformScale, float rotationAboutZDegrees, Vec3 const& translation, Rgba8 const& color)
{
    Mat44 transformMat = Mat44::CreateZRotationDegrees(rotationAboutZDegrees);
    transformMat.ScaleUniform3D(uniformScale);
    transformMat.SetTranslation3D(translation);

    Mat44 normalMat = transformMat;
    normalMat.SetTranslation3D(Vec3::ZERO);
    normalMat = MatrixTranspose(MatrixInverse(normalMat));

    for (int i = 0; i < vertexesNumber; i++) {
        Vertex_PCUTBN& vert = vertexes[i];
        vert.position = transformMat.TransformPosition3D(vert.position);
        vert.tint = color;
        vert.normal = normalMat.TransformVector3D(vert.normal).GetNormalized();
        Vec3 tangentDir = normalMat.TransformVector3D(vert.tangent.GetXYZ()).GetNormalized();
        vert.tangent = Vec4(tangentDir, vert.tangent.w);
    }
}
