#include "Engine/Physics2D/Collision2D.hpp"

//////////////////////////////////////////////////////////////////////////
Collision2D::Collision2D(Collider2D* first, Collider2D* second, Manifold2 mani)
    : me(first)
    , other(second)
    , manifold(mani)
{
}

//////////////////////////////////////////////////////////////////////////
Collision2D Collision2D::GetInverse() const
{
    return Collision2D(other, me, manifold);
}
