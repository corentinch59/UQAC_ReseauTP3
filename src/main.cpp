#include <DuoBoloNetwork/Physics.h>
#include <DuoBoloNetwork/Rendering.h>
#include <DuoBoloNetwork/SceneLoading.h>
#include <DuoBoloShared/ComponentRegistry.h>
#include <DuoBoloShared/Maths.h>
#include <DuoBoloNetwork/InputManager.h>

#include <spdlog/spdlog.h>

#include <raylib.h>
#include <rcamera.h>

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

#define OBJECT_DESTROY_DISTANCE 1000
constexpr float networkRate = 1.f / 30.f;

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
	std::string username;
	program.add_argument("-i", "--ip").help("IP of the remote server to connect to").store_into(ip).default_value(std::string("localhost"));
	program.add_argument("-u", "--user").help("Username of the player to join as").store_into(username).default_value(std::string("p"));
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

	InputManager inputManager;

#ifndef IS_SERVER
	Renderer renderer;

#ifndef WITH_SCE_EDITOR
	ClientGameSessionManager session(world, componentRegistry);
	OnlineClientManager client;
	client.SetListener(&session);
	if (!client.SendConnectionRequest(13333, ip))
	{
		return 1;
	}

#endif
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

	//Init la camera game->Init();

#ifdef IS_SERVER
	LoadSceneFromPath("assets/Scene1.dbs", world, componentRegistry);
#endif

	auto lastFrame = std::chrono::high_resolution_clock::now();

#ifndef IS_SERVER
	while (!WindowShouldClose())
#else
	// La game loop
	while (true)
#endif
	{
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> delta = now - lastFrame;
		float deltaTime = delta.count();

#ifdef IS_SERVER
		//Server ticking
		accumulator += deltaTime;

		while (accumulator >= networkRate)
		{
			// Do networking stuff
			session.Tick(server.GetHost(),*game, networkRate);
			accumulator -= networkRate;
		}
		server.PollEvents();
		
#endif


#ifndef IS_SERVER
		inputManager.PollInputs(deltaTime);

#ifndef WITH_SCE_EDITOR
		game->GlobalUpdate(deltaTime);

		client.PollEvents();
#endif

		// needed for imgui
		BeginDrawing();
		ClearBackground(BLACK);
#endif

#ifndef WITH_SCE_EDITOR
#ifdef IS_SERVER
		solver.Solve(deltaTime);
#endif

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
        worldEditor.Update(deltaTime);
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
