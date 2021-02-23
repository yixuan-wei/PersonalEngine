#include "Engine/Core/AxisConvention.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Mat44.hpp"

//////////////////////////////////////////////////////////////////////////
Vec3 GetWorldUpVector(AxisConvention customConvention)
{
    switch (customConvention)
    {
    case AXIS_XYZ:      return Vec3(0.f,1.f,0.f);
    case AXIS__YZ_X:    return Vec3(0.f,0.f,1.f);
    default:            return Vec3(0.f,1.f,0.f);
    }
}

//////////////////////////////////////////////////////////////////////////
Vec3 GetWorldForwardVector(AxisConvention customConvention)
{
    switch (customConvention)
    {
    case AXIS_XYZ:      return Vec3(0.f, 0.f, -1.f);
    case AXIS__YZ_X:    return Vec3(1.f, 0.f, 0.f);
    default:            return Vec3(0.f, 0.f, -1.f);
    }
}

//////////////////////////////////////////////////////////////////////////
Mat44 GetAxisMatrixToConvention(AxisConvention customConvention)
{
    switch (customConvention)
    {
    case AXIS_XYZ:      return Mat44::IDENTITY;
    case AXIS__YZ_X:    return Mat44(Vec3(0.f, 0.f, -1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f), Vec3::ZERO);
    default:            return Mat44::IDENTITY;
    }
}

//////////////////////////////////////////////////////////////////////////
Mat44 GetAxisMatrixFromConvention(AxisConvention customConvention)
{
    switch (customConvention)
    {
    case AXIS_XYZ:      return Mat44::IDENTITY;
    case AXIS__YZ_X:    return Mat44(Vec3(0.f,-1.f,0.f), Vec3(0.f,0.f,1.f), Vec3(-1.f,0.f,0.f), Vec3::ZERO);
    default:            return Mat44::IDENTITY;
    }
}

