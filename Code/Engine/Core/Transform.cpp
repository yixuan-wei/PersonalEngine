#include "Engine/Core/Transform.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat44.hpp"

//////////////////////////////////////////////////////////////////////////
static Vec3 GetSmallestSameDegreesVector(Vec3 const& vec)
{
    Vec3 result = vec;
    result.x = GetSmallestSameDegrees(vec.x);
    result.y = GetSmallestSameDegrees(vec.y);
    result.z = GetSmallestSameDegrees(vec.z);
    return result;
}

//////////////////////////////////////////////////////////////////////////
void Transform::Translate(Vec3 const& translation)
{
    m_position += translation;
}

//////////////////////////////////////////////////////////////////////////
void Transform::SetScale(Vec3 const& scale)
{
    m_scale = scale;
}

//////////////////////////////////////////////////////////////////////////
void Transform::SetPosition(Vec3 const& pos)
{
    m_position = pos;
}

//////////////////////////////////////////////////////////////////////////
void Transform::SetRotationAroundXYZDegrees(Vec3 const& newRotation)
{
    m_rotationAroundXYZDegrees = GetSmallestSameDegreesVector(newRotation);
}

//////////////////////////////////////////////////////////////////////////
void Transform::SetRotationPitchYawRollDegrees(float pitch,  float yaw, float roll, AxisConvention convention)
{
    Vec3 rawRotation;
    switch (convention)
    {
    case AXIS_XYZ:      rawRotation = Vec3(pitch, yaw, roll);   break;
    case AXIS__YZ_X:    rawRotation = Vec3(roll, pitch, yaw);   break;
    default:            rawRotation = Vec3(pitch, yaw, roll);   break;
    }
    m_rotationAroundXYZDegrees = GetSmallestSameDegreesVector(rawRotation);
}

//////////////////////////////////////////////////////////////////////////
Mat44 Transform::ToMatrix(AxisConvention convention) const
{
    return Mat44::FromScaleRotationTranslation(m_scale, m_rotationAroundXYZDegrees, m_position, convention);
}
