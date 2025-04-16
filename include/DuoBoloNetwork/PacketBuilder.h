#pragma once

#include <entt/entt.hpp>
#include <enet6/enet.h>

#include <DuoBoloShared/ComponentRegistry.h>

class PacketBuilder
{
	public:
		PacketBuilder() = default;

		ENetPacket* build_world_init_packet(entt::registry& world, ComponentRegistry& components);
		ENetPacket* build_player_packet();
};


