#include <DuoBoloShared/Maths.h>

#include <rcamera.h>
#include <rlgl.h>

bool IsSphereInsideCameraFrustum(Camera3D camera, float radius, Vector3 position, float aspect)
{
    // Get the view and projection matrices
    Matrix matView = MatrixLookAt(camera.position, camera.target, camera.up);
    Matrix matProj;

    if (camera.projection == CAMERA_PERSPECTIVE)
        matProj = MatrixPerspective(
            camera.fovy * DEG2RAD, aspect, CAMERA_CULL_DISTANCE_NEAR, CAMERA_CULL_DISTANCE_FAR);
    else if (camera.projection == CAMERA_ORTHOGRAPHIC)
        matProj = MatrixOrtho(
            -camera.fovy * aspect, camera.fovy * aspect, -camera.fovy, camera.fovy, CAMERA_CULL_DISTANCE_NEAR,
            CAMERA_CULL_DISTANCE_FAR);
    Matrix matViewProj = MatrixMultiply(matView, matProj);

    // Extract the frustum planes from the view-projection matrix
    float planes[6][4];
    // Left plane
    planes[0][0] = matViewProj.m3 + matViewProj.m0;
    planes[0][1] = matViewProj.m7 + matViewProj.m4;
    planes[0][2] = matViewProj.m11 + matViewProj.m8;
    planes[0][3] = matViewProj.m15 + matViewProj.m12;

    // Right plane
    planes[1][0] = matViewProj.m3 - matViewProj.m0;
    planes[1][1] = matViewProj.m7 - matViewProj.m4;
    planes[1][2] = matViewProj.m11 - matViewProj.m8;
    planes[1][3] = matViewProj.m15 - matViewProj.m12;

    // Bottom plane
    planes[2][0] = matViewProj.m3 + matViewProj.m1;
    planes[2][1] = matViewProj.m7 + matViewProj.m5;
    planes[2][2] = matViewProj.m11 + matViewProj.m9;
    planes[2][3] = matViewProj.m15 + matViewProj.m13;

    // Top plane
    planes[3][0] = matViewProj.m3 - matViewProj.m1;
    planes[3][1] = matViewProj.m7 - matViewProj.m5;
    planes[3][2] = matViewProj.m11 - matViewProj.m9;
    planes[3][3] = matViewProj.m15 - matViewProj.m13;

    // Near plane
    planes[4][0] = matViewProj.m3 + matViewProj.m2;
    planes[4][1] = matViewProj.m7 + matViewProj.m6;
    planes[4][2] = matViewProj.m11 + matViewProj.m10;
    planes[4][3] = matViewProj.m15 + matViewProj.m14;

    // Far plane
    planes[5][0] = matViewProj.m3 - matViewProj.m2;
    planes[5][1] = matViewProj.m7 - matViewProj.m6;
    planes[5][2] = matViewProj.m11 - matViewProj.m10;
    planes[5][3] = matViewProj.m15 - matViewProj.m14;

    // Normalize the planes
    for (int i = 0; i < 6; i++)
    {
        float length = sqrtf(planes[i][0] * planes[i][0] + planes[i][1] * planes[i][1] + planes[i][2] * planes[i][2]);
        planes[i][0] /= length;
        planes[i][1] /= length;
        planes[i][2] /= length;
        planes[i][3] /= length;
    }

    // Check if the sphere is inside the frustum
    for (int i = 0; i < 6; i++)
    {
        float distance = planes[i][0] * position.x + planes[i][1] * position.y + planes[i][2] * position.z + planes[i][
            3];
        if (distance < -radius)
        {
            return false;
        }
    }

    return true;
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

Vector3 QuaternionRightVector(Quaternion q) {
    Vector3 right;
    
    right.x = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
    right.y = 2.0f * (q.x * q.y + q.w * q.z);
    right.z = 2.0f * (q.x * q.z - q.w * q.y);

    right *= -1;
    
    return right;
}