#ifdef WITH_SCE_EDITOR

#include <DuoBoloNetwork/WorldEditor.h>
#include <DuoBoloNetwork/Rendering.h>
#include <DuoBoloShared/TransformComponent.h>
#include <DuoBoloShared/ComponentRegistry.h>
#include <DuoBoloShared/RenderableComponent.h>
#include <DuoBoloNetwork/SceneLoading.h>

#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <raylib.h>
#include <rlgl.h>
#include <rcamera.h>

#include <fstream>
#include <spdlog/spdlog.h>
#include <fmt/format.h>

#define ImGuiWindowFlags_NoInputIfCamera (mInCameraMode ? ImGuiWindowFlags_NoInputs : ImGuiWindowFlags_None)

WorldEditor::WorldEditor(entt::registry &world, Renderer *renderer, const ComponentRegistry& componentRegistry,  std::shared_ptr<ImGuiSpdlogSinkMt> logSink) :
mEnttWorld(world),
mRenderer(renderer),
mComponentRegistry(componentRegistry),
mSink(logSink)
{
    mInCameraMode = false;

    mCamera = {};
    mCamera.position = {0.0f, 4.0f, 10.0f}; // Camera position
    mCamera.target = {0.0f, 0.0f, 0.0f};    // Camera looking at point
    mCamera.up = {0.0f, 1.0f, 0.0f};        // Camera up vector (rotation towards target)
    mCamera.fovy = 60.0f;                   // Camera field-of-view Y
    mCamera.projection = CAMERA_PERSPECTIVE;

    mDockingSpaceCreated = false;
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

    InspectorWindow();

    if (mInCameraMode) {
        CameraMovement(dt, &mCamera);
    }

    mRenderer->Render(mEnttWorld, mCamera);

    if (mSelected != entt::null) {
        // render selected icon
        if (mEnttWorld.any_of<TransformComponent>(mSelected)) {
            auto &transform = mEnttWorld.get<TransformComponent>(mSelected);
            if (mEnttWorld.any_of<RenderableComponent>(mSelected)) {
                auto &renderable = mEnttWorld.get<RenderableComponent>(mSelected);

                Vector3 axis;
                float angle;
                QuaternionToAxisAngle(transform.rotation, &axis, &angle);

                BeginTextureMode(mRenderer->GetRenderTexture());
                BeginMode3D(mCamera);
                rlDisableDepthTest();
                DrawModelWiresEx(mRenderer->GetModel(renderable.model), transform.position, axis, angle * RAD2DEG, transform.scale, RED);
                rlEnableDepthTest();
                EndMode3D();
                EndTextureMode();
            }

            Vector2 positionOnScreen = GetWorldToScreenEx(transform.position, mCamera, mRenderer->GetWidth(), mRenderer->GetHeight());

            BeginTextureMode(mRenderer->GetRenderTexture());
            DrawCircle(positionOnScreen.x, positionOnScreen.y, 10.f, BLACK);
            DrawCircle(positionOnScreen.x, positionOnScreen.y, 9.f, WHITE);
            EndTextureMode();
        }
    }

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
        ImGuiID topID = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Up, 0.8f, nullptr, &bottomID);
        ImGuiID topLeftID;
        ImGuiID topRightID = ImGui::DockBuilderSplitNode(topID, ImGuiDir_Right, 0.8f, nullptr, &topLeftID);
        ImGuiID topMiddleID;
        topRightID = ImGui::DockBuilderSplitNode(topRightID, ImGuiDir_Right, 0.2f, nullptr, &topMiddleID);

        ImGui::DockBuilderDockWindow(gInspectorWindowName, topRightID);
        ImGui::DockBuilderDockWindow(gViewportWindowName, topMiddleID);
        ImGui::DockBuilderDockWindow(gHierarchyWindowName, topLeftID);
        ImGui::DockBuilderDockWindow(gConsoleWindowName, bottomID);

        ImGui::DockBuilderFinish(dockspaceID);
    }

    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    ImGui::End();
}

bool WorldEditor::EntityInspector(entt::entity entity)
{
    return false;
}

void WorldEditor::MainMenuBar() {
    // menu bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Edit")) {
            
            ImGui::Text("Path");

            ImGui::SameLine();

            ImGui::InputText("##pathInput", &mScenePath);
            if (ImGui::Button("Save scene"))
                SaveScene();

            ImGui::SameLine();

            if (ImGui::Button("Load scene"))
                LoadScene();

            ImGui::EndMenu();
        }

        

        ImGui::EndMenuBar();
    }
}

void WorldEditor::ViewportWindow() {
    ImGui::Begin(gViewportWindowName, nullptr, ImGuiWindowFlags_NoInputIfCamera | ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Options")) {
            ImGui::Checkbox("Auto adapt renderer size", &mAutoAdapt);
            if (mAutoAdapt)
                ImGui::BeginDisabled();
            ImGui::InputInt("Width", &mNewWidth, 1);
            ImGui::InputInt("Height", &mNewHeight, 1);
            if (ImGui::Button("Apply")) {
                mRenderer->SetRenderSize(mNewWidth, mNewHeight);
            }
            if (mAutoAdapt)
                ImGui::EndDisabled();
            ImGui::EndMenu();
        }
        ImGui::Text("Framerate: %i FPS", GetFPS());
        ImGui::EndMenuBar();
    }

    if (mAutoAdapt) {
        auto size = ImGui::GetContentRegionAvail();
        mNewWidth = size.x;
        mNewHeight = size.y;
        mRenderer->SetRenderSize(mNewWidth, mNewHeight);
    }

    RenderTexture2D rt = mRenderer->GetRenderTexture();
    // rlImGuiImageRenderTexture(&rt);

    ImVec2 avail = ImGui::GetContentRegionAvail(); // Available space
    float imgAspect = (float) mRenderer->GetWidth() / (float) mRenderer->GetHeight();
    float availAspect = avail.x / avail.y;

    ImVec2 imageSize;
    if (imgAspect > availAspect) {
        // Image is wider than available space
        imageSize.x = avail.x;
        imageSize.y = avail.x / imgAspect;
    } else {
        // Image is taller than available space
        imageSize.y = avail.y;
        imageSize.x = avail.y * imgAspect;
    }

    ImVec2 cursorPos = ImGui::GetCursorScreenPos(); // Get current cursor position
    ImVec2 centerPos = ImVec2(
        cursorPos.x + (avail.x - imageSize.x) * 0.5f,
        cursorPos.y + (avail.y - imageSize.y) * 0.5f
    );

    ImGui::SetCursorScreenPos(centerPos);
    ImGui::Image((ImTextureID) mRenderer->GetRenderTexture().texture.id, imageSize, ImVec2(0, 1), ImVec2(1, 0));

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
        mInCameraMode = true;
        HideCursor();
        DisableCursor();
    } else if (mInCameraMode && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        mInCameraMode = false;
        ShowCursor();
        EnableCursor();
    }

    ImGui::End();
}

void WorldEditor::HierarchyWindow() {
    ImGui::Begin(gHierarchyWindowName, nullptr, ImGuiWindowFlags_NoInputIfCamera);

    for (auto entity : mEnttWorld.view<entt::entity>()) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (mSelected == entity) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        bool open = ImGui::TreeNodeEx(fmt::format("Entity {}", (uint32_t)entity).c_str(), flags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            spdlog::info("Selected entity {}", (uint32_t)entity);
            mSelected = entity;
        }

        if (open) {
            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void WorldEditor::InspectorWindow() {
    ImGui::Begin(gInspectorWindowName, nullptr, ImGuiWindowFlags_NoInputIfCamera);

    if(mSelected != entt::null)
    {
        entt::handle entityHandle(mEnttWorld, mSelected);
        bool canAddComponent = false;
        mComponentRegistry.ForEachComponent([&](const ComponentRegistry::Entry& entry)
            {
                assert(entry.hasComponent);
                if (entry.hasComponent(entityHandle))
                {
                    if (!ImGui::TreeNode(entry.label.c_str()))
                        return;

                    if (entry.inspect)
                        entry.inspect(*this, entityHandle);

                    if (entry.removeComponent && ImGui::Button(fmt::format("Remove {} component", entry.label).c_str()))
                        entry.removeComponent(entityHandle);

                    ImGui::TreePop();
                }
                else if (entry.addComponent)
                    canAddComponent = true;
            });
    }

    ImGui::End();
}

void WorldEditor::LoadScene()
{
    LoadSceneFromPath(mScenePath, mEnttWorld, mComponentRegistry);
}

void WorldEditor::SaveScene()
{
    SaveSceneToPath(mScenePath, mEnttWorld, mComponentRegistry);
}

template<>
void WorldEditor::PopulateInspector<TransformComponent>(entt::handle entity)
{
    auto& t = entity.get<TransformComponent>();
    float posArray[3] = { t.position.x, t.position.y, t.position.z };
    if (ImGui::InputFloat3("Position", posArray))
    {
        t.position.x = posArray[0];
        t.position.y = posArray[1];
        t.position.z = posArray[2];
    }

    float rotArray[4] = { t.rotation.x, t.rotation.y, t.rotation.z, t.rotation.w };
    if (ImGui::InputFloat3("Rotation Quaternion", rotArray))
    {
        t.rotation.x = rotArray[0];
        t.rotation.y = rotArray[1];
        t.rotation.z = rotArray[2];
        t.rotation.w = rotArray[3];
    }

    float scaleArray[3] = { t.scale.x, t.scale.y, t.scale.z };
    if (ImGui::InputFloat3("Scale", scaleArray))
    {
        t.scale.x = scaleArray[0];
        t.scale.y = scaleArray[1];
        t.scale.z = scaleArray[2];
    }
}

#endif