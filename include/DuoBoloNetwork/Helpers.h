//
// Created by theo on 12/03/2025.
//

#pragma once

#include <LinearMath/btQuaternion.h>
#include <LinearMath/btVector3.h>

#include <raylib.h>

btVector3 ToBtVector3(Vector3 v);

Vector3 ToVector3(btVector3 v);

bool IsEqual(Vector3 rVec, btVector3 btVec);

bool IsEqual(Quaternion rVec, btQuaternion btVec);

bool IsSphereInsideCameraFrustum(Camera3D camera, float radius, Vector3 position, float aspect);

Vector3 QuaternionUpVector(Quaternion q);

Vector3 QuaternionForwardVector(Quaternion q);