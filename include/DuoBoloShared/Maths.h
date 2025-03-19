#pragma once

#include <raylib.h>
#include <raymath.h>

bool IsSphereInsideCameraFrustum(Camera3D camera, float radius, Vector3 position, float aspect);

Vector3 QuaternionUpVector(Quaternion q);

Vector3 QuaternionForwardVector(Quaternion q);

Vector3 QuaternionRightVector(Quaternion q);