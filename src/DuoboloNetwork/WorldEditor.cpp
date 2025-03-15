
#include <DuoBoloNetwork/WorldEditor.h>

#include <imgui.h>
#include <rlImGui.h>
#include <rcamera.h>

WorldEditor::WorldEditor(entt::registry &world) : mEnttWorld(world) {
    mInCameraMode = false;
}

void WorldEditor::Update(float dt, Renderer *renderer, Camera *camera) {
    if (IsKeyPressed(KEY_F10))
    {
        mInCameraMode = !mInCameraMode;

        if (mInCameraMode)
        {
            HideCursor();
        } else {
            ShowCursor();
        }
    }

    if (mInCameraMode)
    {
        CameraMovement(dt, camera);
    }

#define DISABLE_INPUT_CAMERA_MODE mInCameraMode ? ImGuiWindowFlags_NoInputs : ImGuiWindowFlags_None

    ImGui::Begin("RT", nullptr, DISABLE_INPUT_CAMERA_MODE);
    RenderTexture2D rt = renderer->GetRenderTexture();
    rlImGuiImageRenderTextureFit(&rt, true);
    ImGui::End();

//    ImGui::Begin("Console");
//    ImGui::End();
}

void WorldEditor::CameraMovement(float deltaTime, Camera *camera) {
    float cameraMouseSens = 0.003f;
    float cameraSpeed = 5.4f;

    const Vector2 mouseDelta = GetMouseDelta();
    CameraYaw(camera, -mouseDelta.x * cameraMouseSens, false);
    CameraPitch(camera, -mouseDelta.y * cameraMouseSens, true, false, false);
    if (IsKeyDown(KEY_W))
        CameraMoveForward(camera, cameraSpeed * deltaTime, true);
    if (IsKeyDown(KEY_A))
        CameraMoveRight(camera, -cameraSpeed * deltaTime, true);
    if (IsKeyDown(KEY_S))
        CameraMoveForward(camera, -cameraSpeed * deltaTime, true);
    if (IsKeyDown(KEY_D))
        CameraMoveRight(camera, cameraSpeed * deltaTime, true);
    if (IsKeyDown(KEY_Q))
        CameraMoveUp(camera, cameraSpeed * deltaTime);
    if (IsKeyDown(KEY_E))
        CameraMoveUp(camera, -cameraSpeed * deltaTime);
}