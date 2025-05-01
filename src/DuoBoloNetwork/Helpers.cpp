//
// Created by theo on 12/03/2025.
//

#include <DuoBoloNetwork/Helpers.h>

#include <raymath.h>
#include <rcamera.h>
#include <rlgl.h>

btVector3 ToBtVector3(Vector3 v)
{
    return {v.x, v.y, v.z};
}

Vector3 ToVector3(btVector3 v)
{
    return {v.getX(), v.getY(), v.getZ()};
}

bool IsEqual(Vector3 rVec, btVector3 btVec)
{
    return rVec.x == btVec.getX() && rVec.y == btVec.getY() && rVec.z == btVec.getZ();
}

bool IsEqual(Quaternion rVec, btQuaternion btVec)
{
    return rVec.x == btVec.getX() && rVec.y == btVec.getY() && rVec.z == btVec.getZ() && rVec.w == btVec.getW();
}