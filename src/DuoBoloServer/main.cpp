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

#include <curl/curl.h>

#include "DuoBoloShared/PhysicsComponent.h"
#include "DuoBoloShared/UserCreationInfoComponent.h"

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
	for (auto&& [entity] : world.view<entt::entity>().each())
	{
		auto& node = world.emplace<NetworkComponent>(entity);
		node.id = static_cast<std::uint32_t>(entity);
	}

	auto lastFrame = std::chrono::high_resolution_clock::now();
	auto stopTime = std::chrono::high_resolution_clock::now() + std::chrono::minutes(2);

	std::map<uint32_t, uint32_t> cubesDropped;

	while (stopTime > std::chrono::high_resolution_clock::now())
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
			world.view<TransformComponent>().each([&](const auto entity, auto&& transform)
			{
				if (Vector3LengthSqr(transform.position) > OBJECT_DESTROY_DISTANCE * OBJECT_DESTROY_DISTANCE)
				{
					if (world.any_of<RigidbodyComponent>(entity) && !world.any_of<UserCreationInfoComponent>(entity))
					{
						RigidbodyComponent& comp = world.get<RigidbodyComponent>(entity);
						if (comp.lastTouchedByUser != -1)
						{
							if (cubesDropped.contains(comp.lastTouchedByUser))
							{
								cubesDropped[comp.lastTouchedByUser]++;
							}
							else
							{
								cubesDropped[comp.lastTouchedByUser] = 1;
							}
						}
					}

					world.destroy(entity);
				}
			});
		}

		lastFrame = now;
	}

	world.clear();

	// get os env variable MATCH_ID
	try
	{
		std::map<std::string, uint32_t> cubesDroppedByUsername;

		for (auto& [cId, cubes] : cubesDropped)
		{
			std::string user = session.GetUsernameForConnection(cId);
			if (cubesDroppedByUsername.contains(user))
			{
				cubesDroppedByUsername[user] += cubes;
			}
			else
			{
				cubesDroppedByUsername[user] = cubes;
			}
		}

		for (auto& [user, cubes] : cubesDroppedByUsername)
		{
			spdlog::info("{} dropped {} cubes", user, cubes);
		}

		const char* env = std::getenv("MATCH_ID");
		std::string matchId;
		if (env != nullptr) { // OK : on copie la chaîne si elle existe
			matchId = env;
		}
		else {
			spdlog::error("No match ID in env");
		}

		CURL* curl;
		CURLcode res;

		/* In Windows, this inits the Winsock stuff */
		curl_global_init(CURL_GLOBAL_ALL);

		/* get a curl handle */
		curl = curl_easy_init();
		if (curl)
		{
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
			curl_easy_setopt(curl, CURLOPT_URL, fmt::format("https://8v66afx0ff.execute-api.us-east-1.amazonaws.com/prod/server/postmatchresult?matchId={}", matchId).c_str());
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fmt::format("matchId={}", matchId).c_str());

			struct curl_slist* headers = NULL;
			headers = curl_slist_append(headers, "Content-Type: text/json");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

			nlohmann::json j = {
				{"matchId", matchId},
				{"players", nlohmann::json::array()}
			};

			for (auto& [user, cubes] : cubesDroppedByUsername)
			{
				nlohmann::json player = {
					{"playerId", user},
					{"cubesDropped", static_cast<int>(cubes)}
				};

				j["players"].push_back(player);
			}

			std::string data = j.dump();

			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

			/* Perform the request, res gets the return code */
			res = curl_easy_perform(curl);
			/* Check for errors */
			if (res != CURLE_OK)
				spdlog::error("Couldn't post match result...");

			/* always cleanup */
			curl_easy_cleanup(curl);
		}
		curl_global_cleanup();
	}
	catch (std::exception e)
	{
		spdlog::error("Error sending match info: {}", e.what());
	}

	return EXIT_SUCCESS;
}

DUOMain(ServerMain)
