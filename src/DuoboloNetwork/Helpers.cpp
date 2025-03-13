//
// Created by theo on 12/03/2025.
//

#include <DuoBoloNetwork/Helpers.h>

btVector3 ToBtVector3(Vector3 v)
{
    return {v.x, v.y, v.z};
}

Vector3 ToVector3(btVector3 v)
{
    return {v.getX(), v.getY(), v.getZ()};
}
