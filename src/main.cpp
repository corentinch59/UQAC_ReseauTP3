#include <DuoBoloNetwork/Physics.h>
#include <DuoBoloNetwork/Rendering.h>
#include <DuoBoloNetwork/SceneLoading.h>
#include <DuoBoloShared/ComponentRegistry.h>
#include <DuoBoloShared/Maths.h>
#include <DuoBoloGame/BaseGame.h>

#include <spdlog/spdlog.h>

#include <raylib.h>

#include <entt/entt.hpp>


#ifdef WITH_SCE_EDITOR
#include <DuoBoloNetwork/WorldEditor.h>
#include <rlImGui.h>
#else
#include <raymath.h>
#include <DuoBoloShared/CameraComponent.h>
#include <DuoBoloShared/TransformComponent.h>
#include <DuoBoloNetwork/Helpers.h>
#endif

#define OBJECT_DESTROY_DISTANCE 1000

void CustomLogCallback(int logLevel, const char* text, va_list args)
{
	// Format the log message using the variable arguments
	char buffer[512];
	vsnprintf(buffer, sizeof(buffer), text, args);

	// Use spdlog to log the message depending on log level
	switch (logLevel)
	{
	case LOG_FATAL:
		spdlog::critical("[raylib] {}", buffer); // Critical logs for fatal errors
		break;
	case LOG_ERROR:
		spdlog::error("[raylib] {}", buffer); // Error logs
		break;
	case LOG_WARNING:
		spdlog::warn("[raylib] {}", buffer); // Warning logs
		break;
	case LOG_INFO:
		spdlog::info("[raylib] {}", buffer); // Informational logs
		break;
	case LOG_DEBUG:
		spdlog::debug("[raylib] {}", buffer); // Debug logs
		break;
	default:
		spdlog::info("[raylib] Unknown log level: {}", buffer); // Default case
		break;
	}
}

int main()
{
	spdlog::set_level(spdlog::level::debug);

#ifdef WITH_SCE_EDITOR
    // So we can see in the editor the previous logs
    auto logSink = std::make_shared<ImGuiSpdlogSinkMt>();
    spdlog::default_logger()->sinks().push_back(logSink);
#endif

	SetTraceLogCallback(CustomLogCallback);

	InitWindow(1280, 720, "DuoBolo TP3");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	DisableCursor();

	entt::registry world{};

	BaseGame* game = CreateGameClass();
	game->SetWorld(&world);
	// spdlog::critical("registry address inside exe: {}", (void*) &world);

#ifdef WITH_SCE_EDITOR
    rlImGuiSetup(true);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif
	/*
#ifndef WITH_SCE_EDITOR
	// is updated using the values of the components
	Camera camera{};
	camera.position = {0.0f, 4.0f, 10.0f}; // Camera position
	camera.target = {0.0f, 0.0f, 0.0f}; // Camera looking at point
	camera.up = {0.0f, 1.0f, 0.0f}; // Camera up vector (rotation towards target)
	camera.fovy = 60.0f; // Camera field-of-view Y
	camera.projection = CAMERA_PERSPECTIVE;
#endif
*/

	Renderer renderer;
	PhysicsSolver solver(world);
	ComponentRegistry componentRegistry;
	game->RegisterComponents(&componentRegistry);
	game->SetLoadSceneFunc([&](const std::string& path)
	{
		LoadSceneFromPath(path, world, componentRegistry);
	});

#ifdef WITH_SCE_EDITOR
    WorldEditor worldEditor(world, &renderer, componentRegistry, logSink);

    renderer.SetRenderIntoTexture(true);
#endif

	game->Init();
	game->LoadScene(game->GetStartupSceneName());

	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();

		if (IsKeyPressed(KEY_F10))
		{
			if (IsCursorHidden())
			{
				ShowCursor();
				EnableCursor();
			}
			else
			{
				HideCursor();
				DisableCursor();
			}
		}

		// needed for imgui
		BeginDrawing();
		ClearBackground(BLACK);

#ifndef WITH_SCE_EDITOR
		// destroy objects that are too far
		{
			auto view = world.view<TransformComponent>();

			for (auto&& [entity, transform] : view.each())
			{
				if (Vector3LengthSqr(transform.position) > OBJECT_DESTROY_DISTANCE * OBJECT_DESTROY_DISTANCE)
					world.destroy(entity);
			}
		}

		game->GlobalUpdate(deltaTime);

		solver.Solve(deltaTime);


		renderer.Render(world, game->GetCamera());
#else
        rlImGuiBegin();
        worldEditor.Update(deltaTime);
        rlImGuiEnd();
#endif

		EndDrawing();
	}

	world.clear();

	game->Shutdown();

#ifdef WITH_SCE_EDITOR
    rlImGuiShutdown();
#endif
	CloseWindow();

	return 0;
}
