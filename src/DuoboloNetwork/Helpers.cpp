//
// Created by theo on 12/03/2025.
//

#include <DuoBoloNetwork/Helpers.h>

#include <rcamera.h>
#include <rlgl.h>
#include <raylib.h>
#include <raymath.h>

btVector3 ToBtVector3(Vector3 v)
{
    return {v.x, v.y, v.z};
}

Vector3 ToVector3(btVector3 v)
{
    return {v.getX(), v.getY(), v.getZ()};
}

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
