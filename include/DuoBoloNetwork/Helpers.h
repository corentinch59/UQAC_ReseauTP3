//
// Created by theo on 12/03/2025.
//

#pragma once

#include <raylib.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btVector3.h>

btVector3 ToBtVector3(Vector3 v);

Vector3 ToVector3(btVector3 v);

inline bool IsEqual(Vector3 rVec, btVector3 btVec)
{
    return rVec.x == btVec.getX() && rVec.y == btVec.getY() && rVec.z == btVec.getZ();
}

inline bool IsEqual(Quaternion rVec, btQuaternion btVec)
{
    return rVec.x == btVec.getX() && rVec.y == btVec.getY() && rVec.z == btVec.getZ() && rVec.w == btVec.getW();
}

bool IsSphereInsideCameraFrustum(Camera3D camera, float radius, Vector3 position, float aspect);

Vector3 QuaternionUpVector(Quaternion q);

Vector3 QuaternionForwardVector(Quaternion q);