#pragma once

#include <raylib.h>
#include <raymath.h>

struct Plane {
    Vector3 normal;
    float d;

    Plane() {}

    Plane(const Vector3& p1, const Vector3& norm)
        : normal(Vector3Normalize(norm)),
        d(Vector3DotProduct(normal, p1))
    {}
};

struct Frustum
{
    Plane topFace;
    Plane bottomFace;

    Plane rightFace;
    Plane leftFace;

    Plane farFace;
    Plane nearFace;
};


bool IsSphereInsideCameraFrustum(Camera3D camera, float radius, Vector3 position, float aspect);

Vector3 QuaternionUpVector(Quaternion q);

Vector3 QuaternionForwardVector(Quaternion q);

Frustum CreateFrustumFromCamera(const Camera& cam, float aspect);

float GetSignedDistanceToPlane(const Plane& plane, const Vector3& point);

bool IsSphereOnOrForwardPlane(Vector3 position, float radius, const Plane& plane);