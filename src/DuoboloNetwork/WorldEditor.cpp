
#include "DuoBoloNetwork/WorldEditor.h"
#include <DuoBoloNetwork/WorldEditor.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <rlImGui.h>
#include <rcamera.h>

#include <spdlog/spdlog.h>

#define ImGuiWindowFlags_NoInputIfCamera (mInCameraMode ? ImGuiWindowFlags_NoInputs : ImGuiWindowFlags_None)

WorldEditor::WorldEditor(entt::registry &world, Renderer *renderer) : mEnttWorld(world), mRenderer(renderer) {
    mInCameraMode = false;

    mCamera = {};
    mCamera.position = {0.0f, 4.0f, 10.0f}; // Camera position
    mCamera.target = {0.0f, 0.0f, 0.0f};    // Camera looking at point
    mCamera.up = {0.0f, 1.0f, 0.0f};        // Camera up vector (rotation towards target)
    mCamera.fovy = 60.0f;                   // Camera field-of-view Y
    mCamera.projection = CAMERA_PERSPECTIVE;

    mDockingSpaceCreated = false;

    mSink = std::make_shared<ImGuiSpdlogSinkMt>();

    spdlog::default_logger()->sinks().push_back(mSink);
}

void WorldEditor::Update(float dt) {
    if (IsKeyPressed(KEY_F10)) {
        mInCameraMode = !mInCameraMode;

        if (mInCameraMode) {
            HideCursor();
            DisableCursor();
        } else {
            ShowCursor();
            EnableCursor();
        }
    }

    FullscreenDockingSpace();

    ViewportWindow();

    HierarchyWindow();

    if (mInCameraMode) {
        CameraMovement(dt, &mCamera);
    }

    mRenderer->Render(mEnttWorld, mCamera);

    mSink->DrawConsole(dt, gConsoleWindowName, ImGuiWindowFlags_NoInputIfCamera);
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

void WorldEditor::FullscreenDockingSpace() {
    auto viewport = ImGui::GetMainViewport();

    // Submit a window filling the entire viewport
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags hostWindowFlags = 0;
    hostWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
    hostWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    hostWindowFlags |= ImGuiWindowFlags_MenuBar;

    char label[32];
    ImFormatString(label, IM_ARRAYSIZE(label), "WindowOverViewport_%08X", viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin(label, nullptr, hostWindowFlags);
    ImGui::PopStyleVar(3);

    MainMenuBar();

    // Submit the dockspace
    ImGuiID dockspaceID = ImGui::GetID("DockSpace");

    if (!mDockingSpaceCreated) {
        mDockingSpaceCreated = true;

        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

        ImGuiID bottomID;
        ImGuiID topID = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Up, 0.7f, nullptr, &bottomID);
        ImGuiID topLeftID;
        ImGuiID topRightID = ImGui::DockBuilderSplitNode(topID, ImGuiDir_Right, 0.8f, nullptr, &topLeftID);

        ImGui::DockBuilderDockWindow(gViewportWindowName, topRightID);
        ImGui::DockBuilderDockWindow(gHierarchyWindowName, topLeftID);
        ImGui::DockBuilderDockWindow(gConsoleWindowName, bottomID);

        ImGui::DockBuilderFinish(dockspaceID);
    }

    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    ImGui::End();
}

void WorldEditor::MainMenuBar() {
    // menu bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Edit")) {
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void WorldEditor::ViewportWindow() {
    ImGui::Begin(gViewportWindowName, nullptr, ImGuiWindowFlags_NoInputIfCamera | ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::Checkbox("Auto adapt renderer size", &mAutoAdapt);
            if (mAutoAdapt)
                ImGui::BeginDisabled();
            ImGui::InputInt("Width", &mNewWidth, 16);
            ImGui::InputInt("Height", &mNewHeight, 16);
            if (ImGui::Button("Apply"))
            {
                mRenderer->SetRenderSize(mNewWidth, mNewHeight);
            }
            if (mAutoAdapt)
                ImGui::EndDisabled();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (mAutoAdapt)
    {
        auto size = ImGui::GetWindowSize();
        mNewWidth = size.x;
        mNewHeight = size.y;
        mRenderer->SetRenderSize(mNewWidth, mNewHeight);
    }

    RenderTexture2D rt = mRenderer->GetRenderTexture();
    rlImGuiImageRenderTextureFit(&rt, true);
    ImGui::End();
}

void WorldEditor::HierarchyWindow() {
    ImGui::Begin(gHierarchyWindowName, nullptr, ImGuiWindowFlags_NoInputIfCamera);
    ImGui::End();
}