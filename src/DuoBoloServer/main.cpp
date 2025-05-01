#include <raylib.h>
#include <raymath.h>
#include <cstdlib>
#include <entt/entity/registry.hpp>
#include <Main/Main.hpp>

#include <DuoBoloNetwork/Physics.h>
#include <DuoBoloNetwork/Rendering.h>
#include <DuoBoloNetwork/SceneLoading.h>

#include <DuoBoloShared/ComponentRegistry.h>
#include <DuoBoloShared/TransformComponent.h>

#include <DuoBoloServer/OnlineServerManager.h>
#include <DuoBoloServer/ServerGameSessionManager.h>

#include "DuoBoloShared/NetworkComponent.h"

#define OBJECT_DESTROY_DISTANCE 1000
constexpr float networkRate = 1.f / 30.f;

int ServerMain(int argc, char* argv[])
{
	entt::registry world{};

	PhysicsSolver solver(world);
	ComponentRegistry componentRegistry;

	ServerGame game;
	ServerGameSessionManager session(world, componentRegistry);
	OnlineServerManager server(13333, 4);
	server.SetListener(&session);
	double accumulator = 0.0;

	LoadSceneFromPath("assets/Scene1.dbs", world, componentRegistry);

	//Add NetworkComponent to all entities
	for(auto&& [entity] : world.view<entt::entity>().each())
	{
		auto& node = world.emplace<NetworkComponent>(entity);
		node.id = static_cast<std::uint32_t>(entity);
	}

	auto lastFrame = std::chrono::high_resolution_clock::now();

	while (true)
	{
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> delta = now - lastFrame;
		double deltaTime = delta.count();

		//Server ticking
		accumulator += deltaTime;

		while (accumulator >= networkRate)
		{
			// Do networking stuff
			session.Tick(server.GetHost(), game, networkRate);
			accumulator -= networkRate;
		}
		server.PollEvents();

		solver.Solve(deltaTime);

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

		lastFrame = now;
	}

	world.clear();

	return EXIT_SUCCESS;
}

DUOMain(ServerMain)