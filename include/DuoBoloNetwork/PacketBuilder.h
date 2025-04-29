#pragma once

#include <raylib.h>
#include <entt/entt.hpp>
#include <enet6/enet.h>

#include <DuoBoloNetwork/Input.h>
#include <DuoBoloShared/ComponentRegistry.h>


class PacketBuilder
{
	public:
		PacketBuilder() = default;

		ENetPacket* build_world_init_packet(entt::registry& world, ComponentRegistry& components);
		ENetPacket* build_player_packet();
		ENetPacket* build_game_data(entt::registry& world, ComponentRegistry& components);
		ENetPacket* build_client_input(const PlayerInput& inputs);
		ENetPacket* build_client_shoot(Vector3 position, Vector3 forward);
};


