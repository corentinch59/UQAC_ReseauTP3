#include <Main/Main.hpp>

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>
#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

#include <DuoBoloNetwork/InputManager.h>
#include <DuoBoloNetwork/Input.h>
#include <DuoBoloNetwork/Physics.h>
#include <DuoBoloNetwork/Rendering.h>

#include <DuoBoloShared/ComponentRegistry.h>
#include <DuoBoloShared/TransformComponent.h>

#include <DuoBoloClient/ClientGameSessionManager.h>
#include <DuoBoloClient/OnlineClientManager.h>
#include <DuoBoloClient/ClientGame.h>


#define OBJECT_DESTROY_DISTANCE 1000

void SetupInputActions(ClientGame& game, ClientGameSessionManager& session);

int ClientMain(int argc, char* argv[])
{
	argparse::ArgumentParser program("duobologame");

	std::string ip;
	std::string username;
	program.add_argument("-i", "--ip").help("IP of the remote server to connect to").store_into(ip).default_value(std::string("localhost"));
	program.add_argument("-u", "--user").help("Username of the player to join as").store_into(username).default_value(std::string("p"));

	try
	{
		program.parse_args(argc, argv);
	}
	catch (const std::exception& err)
	{
		spdlog::error(err.what());
		return EXIT_FAILURE;
	}

	InitWindow(1280, 720, "DuoBoloGame");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	DisableCursor();

	entt::registry world{};

	PhysicsSolver solver(world);
	ComponentRegistry componentRegistry;
	WorldSettings wSettings{};
	wSettings.lightDirection = Vector3Normalize({ 0.35f, -1.0f, 0.f });
	wSettings.lightColor = WHITE;
	wSettings.ambientColor = GRAY;

	ClientGame game;
	InputManager inputManager;

	game.SetupInput();

	Renderer renderer;

	OnlineClientManager client;
	ClientGameSessionManager session(world, componentRegistry, client);
	client.SetListener(&session);
	if (!client.SendConnectionRequest(13333, ip))
	{
		return EXIT_FAILURE;
	}

	SetupInputActions(game,session);

	auto lastFrame = std::chrono::high_resolution_clock::now();

	while (!WindowShouldClose())
	{
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> delta = now - lastFrame;
		double deltaTime = delta.count();

		inputManager.PollInputs(deltaTime);

		game.UpdateCameraRotation();

		client.PollEvents();

		// needed for imgui
		BeginDrawing();
		ClearBackground(BLACK);

		renderer.Render(world, game.GetCamera(), wSettings);

		DrawFPS(20, 20);

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

		EndDrawing();

		lastFrame = now;
	}

	world.clear();

	CloseWindow();

	return EXIT_SUCCESS;
}

DUOMain(ClientMain)

void SetupInputActions(ClientGame& game, ClientGameSessionManager& session)
{
	InputManager::Instance().BindAction("forward",
		[&](InputAction a)
		{
			CameraMoveForward(&game.GetCamera(), game.GetCameraSpeed() * a.deltaTime, true);
			if (a.state == PressState::Press)
			{
				game.GetPlayerInput().forward = true;
			}

			if (a.state == PressState::Release)
			{
				game.GetPlayerInput().forward = false;
			}

			session.SendInputPacket(game.GetPlayerInput());
		});
	InputManager::Instance().BindAction("left",
		[&](InputAction a)
		{
			CameraMoveRight(&game.GetCamera(), -game.GetCameraSpeed() * a.deltaTime, true);
			if (a.state == PressState::Press)
			{
				game.GetPlayerInput().left = true;
			}

			if (a.state == PressState::Down)
			{
				game.GetPlayerInput().left = false;
			}

			session.SendInputPacket(game.GetPlayerInput());
		});
	InputManager::Instance().BindAction("backward",
		[&](InputAction a)
		{
			CameraMoveForward(&game.GetCamera(), -game.GetCameraSpeed() * a.deltaTime, true);
			if (a.state == PressState::Press)
			{
				game.GetPlayerInput().backward = true;
			}

			if (a.state == PressState::Down)
			{
				game.GetPlayerInput().backward = false;
			}

			session.SendInputPacket(game.GetPlayerInput());
		});
	InputManager::Instance().BindAction("right",
		[&](InputAction a)
		{
			CameraMoveRight(&game.GetCamera(), game.GetCameraSpeed() * a.deltaTime, true);
			if (a.state == PressState::Press)
			{
				game.GetPlayerInput().right = true;
			}

			if (a.state == PressState::Down)
			{
				game.GetPlayerInput().right = false;
			}

			session.SendInputPacket(game.GetPlayerInput());
		});
	InputManager::Instance().BindAction("up",
		[&](InputAction a)
		{
			CameraMoveUp(&game.GetCamera(), game.GetCameraSpeed() * a.deltaTime);
			if (a.state == PressState::Press)
			{
				game.GetPlayerInput().up = true;
			}

			if (a.state == PressState::Down)
			{
				game.GetPlayerInput().up = false;
			}

			session.SendInputPacket(game.GetPlayerInput());
		});
	InputManager::Instance().BindAction("down",
		[&](InputAction a)
		{
			CameraMoveUp(&game.GetCamera(), -game.GetCameraSpeed() * a.deltaTime);
			if (a.state == PressState::Press)
			{
				game.GetPlayerInput().down = true;
			}

			if (a.state == PressState::Down)
			{
				game.GetPlayerInput().down = false;
			}

			session.SendInputPacket(game.GetPlayerInput());
		});
	InputManager::Instance().BindAction("showCursor",
		[](InputAction a)
		{
			if (a.state != PressState::Press)
				return;

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
		});
	InputManager::Instance().BindAction("shoot",
		[&](InputAction a)
		{
			if (a.state == PressState::Press)
			{
				game.GetPlayerInput().shoot = true;
			}

			if (a.state == PressState::Down)
			{
				game.GetPlayerInput().shoot = false;
			}

			session.SendShootPacket(game.GetCamera().position, game.GetCamera().target - game.GetCamera().position);
		});
}