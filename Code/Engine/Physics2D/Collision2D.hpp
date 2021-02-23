#pragma once

#include "Engine/Physics2D/Manifold2.hpp"

class Collider2D;

struct Collision2D 
{
    Collider2D* me = nullptr;
    Collider2D* other = nullptr;
    Manifold2 manifold;

    Collision2D(Collider2D* first, Collider2D* second, Manifold2 mani);

    Collision2D GetInverse() const;
};