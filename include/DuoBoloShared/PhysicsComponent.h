#pragma once

#include <entt/core/any.hpp>
#include <raylib.h>

struct BoxShape
{
    Vector3 extent;
};

struct SphereShape
{
    float radius;
};

struct RigidbodyComponent
{
    float mass = 1.f;
    entt::any shape = BoxShape{{1, 1, 1}};
    Vector3 velocity = {0, 0, 0};
};