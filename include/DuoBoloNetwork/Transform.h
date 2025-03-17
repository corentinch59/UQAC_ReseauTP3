//
// Created by theo on 12/03/2025.
//

#pragma once

#include <raymath.h>

struct TransformComponent
{
    Vector3 position = {0, 0, 0};
    Quaternion rotation = QuaternionIdentity();
    Vector3 scale = {1, 1, 1}; // only affects rendering !!
};