#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"

//////////////////////////////////////////////////////////////////////////
Mat44 MakeOrthographicProjectionMatrixD3D(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
{
    //Range map x,y to -1.f to 1.f, z to 0 to 1.f
    float rangeX = maxX-minX;
    float rangeY = maxY-minY;
    float rangeZ = maxZ-minZ;
    float mat[] = {
        2.f / rangeX,	0.f,			0.f,			0.f,
        0.f,			2.f / rangeY,	0.f,			0.f,
        0.f,			0.f,			2.f / rangeZ,	0.f,
        -(minX + maxX) / rangeX,	-(minY + maxY) / rangeY,	-(minZ + maxZ) / rangeZ,	1.f
    };
    return Mat44(mat);
}

//////////////////////////////////////////////////////////////////////////
Mat44 MakePerspectiveProjectionMatrix3D(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
    float height = 1.f / TanDegrees(fovDegrees * .5f);
    float zRange = farZ - nearZ;
    zRange = zRange == 0.f ? 1.f : zRange;
    float q = 1.f / zRange;
    aspectRatio = aspectRatio == 0.f ? 1.f : aspectRatio;

    float mat[] = {
        height/aspectRatio, 0,      0,                      0,
        0,                  height, 0,                      0,
        0,                  0,      -farZ*q,                -1.f,
        0,                  0,      nearZ* farZ* q,         0
    };
    return Mat44(mat);
}

//////////////////////////////////////////////////////////////////////////
bool MatrixIsOrthoNormal(Mat44 const& mat)
{
    Vec4 i = mat.GetIBasis4D();
    Vec4 j = mat.GetJBasis4D();
    Vec4 k = mat.GetKBasis4D();
    Vec4 w = mat.GetTranslation4D();
    if (   NearlyEqual(i.GetLengthSquared(), 1.f)   && NearlyEqual(j.GetLengthSquared(), 1.f)
        && NearlyEqual(k.GetLengthSquared(), 1.f)   && NearlyEqual(w.GetLengthSquared(), 1.f)
        && NearlyEqual(DotProduct4D(i, j), 0.f)     && NearlyEqual(DotProduct4D(i, k), 0.f) 
        && NearlyEqual(DotProduct4D(i, w), 0.f)     && NearlyEqual(DotProduct4D(j, k), 0.f)
        && NearlyEqual(DotProduct4D(j, w), 0.f)     && NearlyEqual(DotProduct4D(k, w), 0.f)) {
        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
Mat44 MatrixTranspose(Mat44 const& mat)
{
    Mat44 transpose = mat;
    float* start = &transpose.Ix;
    for (int i = 0; i < 3; i++) {
        for (int j = i + 1; j < 4; j++) {
            int idxA = 4 * i + j;
            int idxB = 4 * j + i;
            float temp = start[idxA];
            start[idxA] = start[idxB];
            start[idxB] = temp;
        }
    }
    return transpose;
}

//////////////////////////////////////////////////////////////////////////
Mat44 MatrixInverseOrthoNormal(Mat44 const& mat)
{
    if (!MatrixIsOrthoNormal(mat)) {
        return MatrixInverse(mat);
    }

    Mat44 inverse = mat;
    inverse.SetTranslation3D(Vec3::ZERO);
    inverse.Transpose();

    Vec3 translation = mat.GetTranslation3D();
    Vec3 invTrans = inverse.TransformPosition3D(-translation);
    inverse.SetTranslation3D(invTrans);

    return inverse;
}

//////////////////////////////////////////////////////////////////////////
Mat44 MatrixInverse(Mat44 const& mat)
{
    if (MatrixIsOrthoNormal(mat)) {
        return MatrixInverseOrthoNormal(mat);
    }

    double inv[16];
    double det = 0;
    double m[16];

    float const* start = &mat.Ix;
    for (int i = 0; i < 16; i++) {
        m[i] = (double)start[i];
    }

    inv[0] = m[5]   * m[10] * m[15] +
             m[9]   * m[14] * m[7] +
             m[13]  * m[6]  * m[11] -
             m[5]   * m[11] * m[14] -
             m[6]   * m[9]  * m[15] -
             m[7]   * m[10] * m[13];

    inv[1] = m[1]   * m[11] * m[14] +
             m[2]   * m[9]  * m[15] +
             m[3]   * m[10] * m[13] -
             m[1]   * m[10] * m[15] -
             m[2]   * m[11] * m[13] -
             m[3]   * m[9]  * m[14];

    inv[2] = m[1]   * m[6]  * m[15] +
             m[2]   * m[7]  * m[13] +
             m[3]   * m[5]  * m[14] -
             m[1]   * m[7]  * m[14] -
             m[2]   * m[5]  * m[15] -
             m[3]   * m[6]  * m[13];

    inv[3] = m[1]   * m[7]  * m[10] +
             m[2]   * m[5]  * m[11] +
             m[3]   * m[6]  * m[9] -
             m[1]   * m[6]  * m[11] -
             m[2]   * m[7]  * m[9] -
             m[3]   * m[5]  * m[10];

    inv[4] = m[4]   * m[11] * m[14] +
             m[6]   * m[8]  * m[15] +
             m[7]   * m[10] * m[12] -
             m[4]   * m[10] * m[15] -
             m[6]   * m[11] * m[12] -
             m[7]   * m[8]  * m[14];

    inv[5] = m[0] * m[10] * m[15] -
             m[0] * m[14] * m[11] -
             m[2] * m[8]  * m[15] +
             m[2] * m[12] * m[11] +
             m[3] * m[8]  * m[14] -
             m[3] * m[12] * m[10];

    inv[6] = -m[0] * m[6] * m[15] +
             m[0] * m[14] * m[7] +
             m[2] * m[4]  * m[15] -
             m[2] * m[12] * m[7] -
             m[3] * m[4]  * m[14] +
             m[3] * m[12] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[10] * m[7] -
             m[2] * m[4] * m[11] +
             m[2] * m[8] * m[7] +
             m[3] * m[4] * m[10] -
             m[3] * m[8] * m[6];

    inv[8] = m[4] * m[9] * m[15] -
             m[4] * m[13] * m[11] -
             m[5] * m[8] * m[15] +
             m[5] * m[12] * m[11] +
             m[7] * m[8] * m[13] -
             m[7] * m[12] * m[9];

    inv[9] = -m[0] * m[9] * m[15] +
             m[0] * m[13] * m[11] +
             m[1] * m[8] * m[15] -
             m[1] * m[12] * m[11] -
             m[3] * m[8] * m[13] +
             m[3] * m[12] * m[9];

    inv[10] = m[0] * m[5] * m[15] -
              m[0] * m[13] * m[7] -
              m[1] * m[4] * m[15] +
              m[1] * m[12] * m[7] +
              m[3] * m[4] * m[13] -
              m[3] * m[12] * m[5];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[9] * m[7] +
               m[1] * m[4] * m[11] -
               m[1] * m[8] * m[7] -
               m[3] * m[4] * m[9] +
               m[3] * m[8] * m[5];

    inv[12] = -m[4] * m[9] * m[14] +
               m[4] * m[13] * m[10] +
               m[5] * m[8] * m[14] -
               m[5] * m[12] * m[10] -
               m[6] * m[8] * m[13] +
               m[6] * m[12] * m[9];

    inv[13] = m[0] * m[9] * m[14] -
              m[0] * m[13] * m[10] -
              m[1] * m[8] * m[14] +
              m[1] * m[12] * m[10] +
              m[2] * m[8] * m[13] -
              m[2] * m[12] * m[9];

    inv[14] = -m[0] * m[5] * m[14] +
               m[0] * m[13] * m[6] +
               m[1] * m[4] * m[14] -
               m[1] * m[12] * m[6] -
               m[2] * m[4] * m[13] +
               m[2] * m[12] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[9] * m[6] -
              m[1] * m[4] * m[10] +
              m[1] * m[8] * m[6] +
              m[2] * m[4] * m[9] -
              m[2] * m[8] * m[5];

    det = m[0] * inv[0] + m[4] * inv[1] + m[8] * inv[2] + m[12] * inv[3];
    det = 1.0 / det;

    Mat44 ret;
    float* retStart = &ret.Ix;
    for (int i = 0; i < 16; i++) {
        retStart[i] = (float)(inv[i] * det);
    }

    return ret;
}

//////////////////////////////////////////////////////////////////////////
Mat44 MatrixMultiply(Mat44 const& matFront, Mat44 const& matBack)
{
    Mat44 result = matFront;
    result.MultiplyRight(matBack);
    return result;
}

//////////////////////////////////////////////////////////////////////////
Mat44 MatrixComponentLerp(Mat44 const& matA, Mat44 const& matB, float value)
{
    Mat44 result;
    float* resultStart = &result.Ix;
    float const* aStart = &matA.Ix;
    float const* bStart = &matB.Ix;
    for (int i = 0; i < 16; i++) {
        resultStart[i] = Interpolate(aStart[i], bStart[i], value);
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
Mat44 MatrixLookAt(Vec3 const& start, Vec3 const& end, Vec3 const& worldUp, Vec3 const& worldForward)
{
    Vec3 direction = end - start;
    Vec3 forward = direction.GetNormalized();
    Vec3 right = CrossProduct3D(forward, worldUp);
    if (NearlyEqual(right.GetLengthSquared(), 0.f)) {
        right = CrossProduct3D(forward, worldForward);
        if (NearlyEqual(right.GetLengthSquared(), 0.f)) {
            right = CrossProduct3D(worldUp, worldForward);
        }
    }
    Vec3 up = CrossProduct3D(right,forward);

    Mat44 lookat;
    lookat.SetBasisVectors3D(right, up, -forward, start);
    return lookat;
}
