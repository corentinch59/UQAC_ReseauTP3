#include <DuoBoloNetwork/Physics.h>
#include <DuoBoloNetwork/Rendering.h>
#include <DuoBoloNetwork/SceneLoading.h>
#include <DuoBoloShared/ComponentRegistry.h>
#include <DuoBoloShared/Maths.h>
#include <DuoBoloGame/BaseGame.h>

#include <spdlog/spdlog.h>

#include <raylib.h>

#include <entt/entt.hpp>

#include <string>
#include <argparse/argparse.hpp>

#ifdef IS_SERVER
#include <DuoBoloNetwork/OnlineServerManager.h>
#include <DuoBoloNetwork/ServerGameSessionManager.h>
#else
#include <DuoBoloNetwork/OnlineClientManager.h>
#include <DuoBoloNetwork/ClientGameSessionManager.h>
#endif

#ifdef WITH_SCE_EDITOR
#ifdef IS_SERVER
#error "Cannot build editor as server"
#endif
#endif


#ifdef WITH_SCE_EDITOR
#include <DuoBoloNetwork/WorldEditor.h>
#include <rlImGui.h>
#else
#include <raymath.h>
#include <DuoBoloShared/TransformComponent.h>
#endif

#include <iostream>

#define OBJECT_DESTROY_DISTANCE 1000
constexpr float networkRate = 1.f / 64.f;

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

int main(int argc, char* argv[])
{
	spdlog::set_level(spdlog::level::debug);

	argparse::ArgumentParser program("duobologame");
	
#ifndef IS_SERVER
	std::string ip;
	program.add_argument("-i", "--ip").help("IP of the remote server to connect to").store_into(ip).required();
#endif

	try
	{
		program.parse_args(argc, argv);
	}
	catch (const std::exception& err)
	{
		spdlog::error(err.what());
		return 1;
	}

#ifndef IS_SERVER
	spdlog::info("Connecting to {}", ip);
#endif

#ifdef WITH_SCE_EDITOR
    // So we can see in the editor the previous logs
    auto logSink = std::make_shared<ImGuiSpdlogSinkMt>();
    spdlog::default_logger()->sinks().push_back(logSink);
#endif

#ifndef IS_SERVER
	SetTraceLogCallback(CustomLogCallback);

	InitWindow(1280, 720, "DuoBolo TP3");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	DisableCursor();
#endif

	entt::registry world{};

	BaseGame* game = CreateGameClass();
	game->SetWorld(&world);
	// spdlog::critical("registry address inside exe: {}", (void*) &world);

#ifdef WITH_SCE_EDITOR
    rlImGuiSetup(true);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif


	PhysicsSolver solver(world);
	ComponentRegistry componentRegistry;
	WorldSettings wSettings{};
	wSettings.lightDirection = Vector3Normalize({0.35f, -1.0f, 0.f});
	wSettings.lightColor = WHITE;
	wSettings.ambientColor = GRAY;
	game->RegisterComponents(&componentRegistry);
	game->SetLoadSceneFunc([&](const std::string& path)
	{
		LoadSceneFromPath(path, world, componentRegistry);
	});

#ifndef IS_SERVER
	Renderer renderer;
	ClientGameSessionManager session(world, componentRegistry);
	OnlineClientManager client;
	client.SetListener(&session);
	if (!client.SendConnectionRequest(13333, ip))
	{
		return 1;
	}

#endif

#ifdef IS_SERVER
	ServerGameSessionManager session(world, componentRegistry);
	OnlineServerManager server(13333, 4);
	server.SetListener(&session);
	double accumulator = 0.0;
#endif

#ifdef WITH_SCE_EDITOR
    WorldEditor worldEditor(world, renderer, wSettings, componentRegistry, logSink);

    renderer.SetRenderIntoTexture(true);
#endif

	game->Init();
#ifdef IS_SERVER
	game->LoadScene(game->GetStartupSceneName());
#endif

	auto lastFrame = std::chrono::high_resolution_clock::now();

#ifndef IS_SERVER
	while (!WindowShouldClose())
#else
	while (true)
#endif
	{
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> delta = now - lastFrame;
		float deltaTime = delta.count();
		float deltaTimeAfterScale = deltaTime * game->GetTimeScale();

#ifdef IS_SERVER
		//Server ticking
		accumulator += deltaTime;

		while (accumulator >= networkRate)
		{
			// Do networking stuff
			accumulator -= networkRate;
		}
		server.PollEvents();
		
#endif

#ifndef IS_SERVER
		client.PollEvents();

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
#endif

#ifndef WITH_SCE_EDITOR
		game->GlobalUpdate(deltaTimeAfterScale);

		solver.Solve(deltaTimeAfterScale);

#ifndef IS_SERVER
		renderer.Render(world, game->GetCamera(), wSettings);

		DrawFPS(20, 20);
#endif

		{
			auto view = world.view<TransformComponent>();

			for (auto&& [entity, transform] : view.each())
			{
				if (Vector3LengthSqr(transform.position) > OBJECT_DESTROY_DISTANCE * OBJECT_DESTROY_DISTANCE)
				{
					world.destroy(entity);
				}
			}
		}
#else
        rlImGuiBegin();
        worldEditor.Update(deltaTimeAfterScale);
        rlImGuiEnd();
#endif

#ifndef IS_SERVER
		EndDrawing();
#endif

		lastFrame = now;
	}

	world.clear();

	game->Shutdown();

#ifdef WITH_SCE_EDITOR
    rlImGuiShutdown();
#endif

#ifndef IS_SERVER
	CloseWindow();
#endif

	return 0;
}
