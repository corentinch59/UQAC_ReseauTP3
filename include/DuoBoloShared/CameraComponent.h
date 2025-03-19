#pragma once

#include <raylib.h>

struct CameraComponent
{
    float fovy = 60.f;
    int projection = CAMERA_PERSPECTIVE;
    bool isMainCamera = true;
};