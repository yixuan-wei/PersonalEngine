#pragma once

#include "Engine/Math/Vec3.hpp"

struct Mat44;

Mat44 MakeOrthographicProjectionMatrixD3D(
    float minX, float maxX,
    float minY, float maxY,
    float minZ, float maxZ);

Mat44 MakePerspectiveProjectionMatrix3D(
    float fovDegrees,
    float aspectRatio,
    float nearZ,
    float farZ
);

bool MatrixIsOrthoNormal(Mat44 const& mat);

Mat44 MatrixTranspose(Mat44 const& mat);
Mat44 MatrixInverseOrthoNormal(Mat44 const& mat);
Mat44 MatrixInverse(Mat44 const& mat);

Mat44 MatrixMultiply(Mat44 const& matFront, Mat44 const& matBack);
Mat44 MatrixComponentLerp(Mat44 const& matA, Mat44 const& matB, float value);

Mat44 MatrixLookAt(Vec3 const& start, Vec3 const& end, Vec3 const& worldUp = Vec3(0.f,1.f,0.f), Vec3 const& worldForwar = Vec3(0.f,0.f,1.f));
