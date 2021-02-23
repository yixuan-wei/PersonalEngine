#pragma once

struct Vec3;
struct Mat44;

enum AxisConvention
{
    AXIS_XYZ,
    AXIS__YZ_X
};

Vec3    GetWorldUpVector(AxisConvention customConvention);
Vec3    GetWorldForwardVector(AxisConvention customConvention);
Mat44   GetAxisMatrixToConvention(AxisConvention customConvention);
Mat44   GetAxisMatrixFromConvention(AxisConvention customConvention);