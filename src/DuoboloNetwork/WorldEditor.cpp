#ifdef WITH_SCE_EDITOR

#include <DuoBoloNetwork/WorldEditor.h>
#include <DuoBoloNetwork/Rendering.h>
#include <DuoBoloNetwork/Transform.h>
#include <DuoBoloNetwork/ComponentRegistry.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <rlImGui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <raylib.h>
#include <rlgl.h>
#include <rcamera.h>

#include <fstream>
#include <spdlog/spdlog.h>
#include <fmt/format.h>

#define ImGuiWindowFlags_NoInputIfCamera (mInCameraMode ? ImGuiWindowFlags_NoInputs : ImGuiWindowFlags_None)
constexpr unsigned int FileVersion = 1;

WorldEditor::WorldEditor(entt::registry &world, Renderer *renderer, const ComponentRegistry& componentRegistry) :
mEnttWorld(world),
mRenderer(renderer),
mComponentRegistry(componentRegistry)
{
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
    ImGui::End();
}

void WorldEditor::LoadScene()
{
    std::ifstream inputFile(mScenePath);
    if (!inputFile)
    {
        spdlog::error("failed to open {}\n", mScenePath);
        return;
    }

    nlohmann::json sceneDoc;
    try
    {
        sceneDoc = nlohmann::json::parse(inputFile);
    }
    catch (const std::exception& e)
    {
        spdlog::error("failed to parse {}: {}\n", mScenePath, e.what());
        return;
    }

    unsigned int version = sceneDoc["Version"];
    if (version > FileVersion)
    {
        spdlog::error("{} has an unknown file version {}\n", mScenePath, version);
        return;
    }

    // On r�initialise le monde pour cr�er les entit�s du document
    mEnttWorld = entt::registry{};

    std::vector<entt::entity> indexToEntity;
    for (const nlohmann::json& entityDoc : sceneDoc["Entities"])
    {
        // Cr�ation de l'entit�
        entt::handle entityHandle(mEnttWorld, mEnttWorld.create());
        indexToEntity.push_back(entityHandle);

        mComponentRegistry.ForEachComponent([&](const ComponentRegistry::Entry& entry)
            {
                if (!entry.jsonUnserialize)
                    return;

                auto it = entityDoc.find(entry.id);
                if (it != entityDoc.end())
                    entry.jsonUnserialize(entityHandle, it.value());
            });
    }

    spdlog::info("successfully loaded {}\n", mScenePath);
}

void WorldEditor::SaveScene()
{
    std::ofstream fileStream(mScenePath);
    if (!fileStream)
    {
        spdlog::error("failed to open {}\n", mScenePath);
        return;
    }

    std::unordered_map<entt::entity, unsigned int> entityToIndex;

    // On sauvegarde tous les composants de toutes les entit�s
    nlohmann::json entityArray;
    for (auto [entity] : mEnttWorld.storage<entt::entity>().each())
    {
        entt::handle entityHandle(mEnttWorld, entity);

        nlohmann::json entityDoc;
        mComponentRegistry.ForEachComponent([&](const ComponentRegistry::Entry& entry)
            {
                assert(entry.hasComponent);
                if (entry.hasComponent(entityHandle) && entry.jsonSerialize)
                    entityDoc[entry.id] = entry.jsonSerialize(entityHandle);
            });


        entityToIndex[entity] = entityArray.size();
        entityArray.push_back(std::move(entityDoc));
    }

    nlohmann::ordered_json sceneDoc;
    sceneDoc["Version"] = FileVersion;
    sceneDoc["Entities"] = std::move(entityArray);

    fileStream << sceneDoc.dump(1, '\t');

    spdlog::info("scene saved to {}\n", mScenePath);
}

#endif