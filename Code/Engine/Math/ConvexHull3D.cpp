#include "Engine/Math/ConvexHull3D.hpp"

//////////////////////////////////////////////////////////////////////////
bool ConvexHull3D::IsPointInside(Vec3 const& point) const
{
    for (Plane3D const& plane : m_boundingPlanes) {
        if (plane.IsPointInFront(point)) {
            return false;
        }
    }

    return true;
}
