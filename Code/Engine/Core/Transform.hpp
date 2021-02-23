#pragma once

#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/AxisConvention.hpp"

struct Mat44;

class Transform 
{
public:
    void Translate(Vec3 const& translation);
    void SetScale(Vec3 const& scale);
    void SetPosition(Vec3 const& pos);
    void SetRotationAroundXYZDegrees(Vec3 const& newRotation);
    void SetRotationPitchYawRollDegrees(float pitch, float yaw, float roll, AxisConvention convention = AXIS_XYZ);

    Mat44 ToMatrix(AxisConvention convention = AXIS_XYZ) const;

public:
    Vec3 m_position                     = Vec3(0.f);
    Vec3 m_rotationAroundXYZDegrees     = Vec3(0.f);
    Vec3 m_scale                        = Vec3(1.f);   
};