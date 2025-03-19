#pragma once

#ifdef WITH_SCE_EDITOR

#include <DuoBoloNetwork/Rendering.h>
#include <DuoBoloNetwork/ImGuiSpdlogSink.h>

#include <imgui.h>
#include <entt/entt.hpp>
#include <entt/entity/fwd.hpp>
#include <raylib.h>

#include <memory>

class ComponentRegistry;

class WorldEditor {
  public:
    WorldEditor(entt::registry &world, Renderer *renderer, const ComponentRegistry& componentRegistry, std::shared_ptr<ImGuiSpdlogSinkMt> logSink);
    WorldEditor(const WorldEditor&) = delete;
    WorldEditor(WorldEditor&&) = delete;

    WorldEditor& operator=(const WorldEditor&) = delete;
    WorldEditor& operator=(WorldEditor&&) = delete;

    void Update(float dt);

  private:
    entt::registry &mEnttWorld;
    Renderer *mRenderer;
    bool mInCameraMode;
    Camera3D mCamera;
    int mNewWidth = 1280;
    int mNewHeight = 720;
    bool mAutoAdapt = true;
    entt::entity mSelected = entt::null;
    std::string mScenePath;
    const ComponentRegistry& mComponentRegistry;

    static inline const char *gViewportWindowName = "Viewport";
    static inline const char *gConsoleWindowName = "Console";
    static inline const char *gHierarchyWindowName = "Hierarchy";
    static inline const char *gInspectorWindowName = "Inspector";

    void CameraMovement(float dt, Camera *camera);

    void FullscreenDockingSpace();

    bool mDockingSpaceCreated;

    void MainMenuBar();

    void ViewportWindow();

    void HierarchyWindow();

    void InspectorWindow();

    void LoadScene();

    void SaveScene();

    // logs
    std::shared_ptr<ImGuiSpdlogSinkMt> mSink;
};

#endif