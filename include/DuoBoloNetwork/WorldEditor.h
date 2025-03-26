#pragma once

#ifdef WITH_SCE_EDITOR

#include <DuoBoloNetwork/ImGuiSpdlogSink.h>

#include <entt/entity/fwd.hpp>
#include <entt/entity/entity.hpp>
#include <raylib.h>

#include <memory>

class Renderer;
class ComponentRegistry;
struct WorldSettings;

class WorldEditor
{
public:
	WorldEditor(entt::registry& world, Renderer& renderer, WorldSettings& wSettings, const ComponentRegistry& componentRegistry,
	            std::shared_ptr<ImGuiSpdlogSinkMt> logSink);
	WorldEditor(const WorldEditor&) = delete;
	WorldEditor(WorldEditor&&) = delete;

	~WorldEditor() = default;

	WorldEditor& operator=(const WorldEditor&) = delete;
	WorldEditor& operator=(WorldEditor&&) = delete;

	void Update(float dt);

	template <typename T>
	void PopulateInspector(entt::handle entity);

private:
	entt::registry& mEnttWorld;
	Renderer* mRenderer;
	WorldSettings* mWSettings;
	bool mInCameraMode;
	Camera3D mCamera;
	int mNewWidth = 1280;
	int mNewHeight = 720;
	bool mAutoAdapt = true;
	entt::entity mSelected = entt::null;
	std::string mScenePath;
	const ComponentRegistry& mComponentRegistry;
	Texture mCubeImage;

	static inline const char* gViewportWindowName = "Viewport";
	static inline const char* gConsoleWindowName = "Console";
	static inline const char* gHierarchyWindowName = "Hierarchy";
	static inline const char* gInspectorWindowName = "Inspector";
	static inline const char* gShapeBrowserWindowName = "Shape browser";

	void CameraMovement(float dt, Camera* camera);

	void FullscreenDockingSpace();

	bool mDockingSpaceCreated;

	bool EntityInspector(entt::entity entity);

	void MainMenuBar();

	void ViewportWindow();

	void HierarchyWindow();

	void InspectorWindow();

	void ShapeBrowserWindow();

	void LoadScene();

	void SaveScene();

	// logs
	std::shared_ptr<ImGuiSpdlogSinkMt> mSink;
};

#endif
