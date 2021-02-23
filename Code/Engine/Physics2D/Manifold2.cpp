#include "Engine/Physics2D/Manifold2.hpp"

//////////////////////////////////////////////////////////////////////////
Manifold2::Manifold2(Vec2 contact1, Vec2 contact2, Vec2 normalDir, float penetrateLength)
    :normal(normalDir)
    ,penetration(penetrateLength)
    ,contact(contact1,contact2)
{
}

//////////////////////////////////////////////////////////////////////////
Manifold2::Manifold2(Vec2 c, Vec2 normalDir, float penetrateLength)
    :normal(normalDir)
    ,contact(c,c)
    ,penetration(penetrateLength)
{
}

//////////////////////////////////////////////////////////////////////////
Manifold2::Manifold2(Manifold2 const& manifold)
    :penetration(manifold.penetration)
    ,normal(manifold.normal)
    ,contact(manifold.contact)
{
}

//////////////////////////////////////////////////////////////////////////
Vec2 Manifold2::GetContact() const
{
    return contact.GetCenter();
}
