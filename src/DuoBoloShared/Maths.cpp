#include <DuoBoloShared/Maths.h>

#include <rcamera.h>
#include <rlgl.h>

bool IsSphereInsideCameraFrustum(Camera3D camera, float radius, Vector3 position, float aspect)
{
    return true;

    Frustum camFrustum = CreateFrustumFromCamera(camera, aspect);

    return (IsSphereOnOrForwardPlane(position, radius, camFrustum.leftFace) &&
        IsSphereOnOrForwardPlane(position, radius, camFrustum.rightFace) &&
        IsSphereOnOrForwardPlane(position, radius, camFrustum.farFace) &&
        IsSphereOnOrForwardPlane(position, radius, camFrustum.nearFace) &&
        IsSphereOnOrForwardPlane(position, radius, camFrustum.topFace) &&
        IsSphereOnOrForwardPlane(position, radius, camFrustum.bottomFace));
}

Vector3 QuaternionUpVector(Quaternion q) {
    Vector3 up;

    up.x = 2.0f * (q.x * q.y + q.w * q.z);
    up.y = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
    up.z = 2.0f * (q.y * q.z - q.w * q.x);

    return up;
}

Vector3 QuaternionForwardVector(Quaternion q) {
    Vector3 forward;

    forward.x = 2.0f * (q.x * q.z - q.w * q.y);
    forward.y = 2.0f * (q.y * q.z + q.w * q.x);
    forward.z = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);

    return forward;
}

Frustum CreateFrustumFromCamera(const Camera& cam, float aspect)
{
    Frustum frustum;

    // Extract camera properties
    Vector3 position = cam.position;
    Vector3 forward = Vector3Normalize(cam.target - cam.position);
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, cam.up));
    Vector3 up = Vector3Normalize(Vector3CrossProduct(right, forward));

    float nearDist = CAMERA_CULL_DISTANCE_NEAR;
    float farDist = CAMERA_CULL_DISTANCE_FAR;
    float fovY = cam.fovy * DEG2RAD;

    if (cam.projection == CAMERA_PERSPECTIVE)
    {
        float halfVSide = tanf(fovY / 2.0f) * nearDist;
        float halfHSide = halfVSide * aspect;

        // Near and far plane centers
        Vector3 nearCenter = position + forward * nearDist;
        Vector3 farCenter = position + forward * farDist;

        // Compute frustum planes
        frustum.nearFace = Plane(nearCenter, forward);
        frustum.farFace = Plane(farCenter, Vector3Negate(forward));

        Vector3 topNormal = Vector3Normalize(Vector3CrossProduct(right, nearCenter + up * halfVSide - position));
        frustum.topFace = Plane(position, topNormal);

        Vector3 bottomNormal = Vector3Normalize(Vector3CrossProduct(nearCenter - up * halfVSide - position, right));
        frustum.bottomFace = Plane(position, bottomNormal);

        Vector3 rightNormal = Vector3Normalize(Vector3CrossProduct(nearCenter + right * halfHSide - position, up));
        frustum.rightFace = Plane(position, rightNormal);

        Vector3 leftNormal = Vector3Normalize(Vector3CrossProduct(up, nearCenter - right * halfHSide - position));
        frustum.leftFace = Plane(position, leftNormal);
    }
    else // Orthographic projection
    {
        float orthoHeight = cam.fovy;
        float orthoWidth = orthoHeight * aspect;

        frustum.nearFace = Plane(position + forward * nearDist, forward);
        frustum.farFace = Plane(position + forward * farDist, Vector3Negate(forward));
        frustum.topFace = Plane(position + up * (orthoHeight / 2.0f), up);
        frustum.bottomFace = Plane(position - up * (orthoHeight / 2.0f), Vector3Negate(up));
        frustum.rightFace = Plane(position + right * (orthoWidth / 2.0f), right);
        frustum.leftFace = Plane(position - right * (orthoWidth / 2.0f), Vector3Negate(right));
    }

    return frustum;
}


float GetSignedDistanceToPlane(const Plane& plane, const Vector3& point)
{
    return Vector3DotProduct(plane.normal, point) - plane.d;
}

bool IsSphereOnOrForwardPlane(Vector3 position, float radius, const Plane& plane)
{
    return GetSignedDistanceToPlane(plane, position) > -radius;
};