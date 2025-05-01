#pragma once

#include <DuoBoloShared/ComponentRegistry.h>
#include <DuoBoloNetwork/PacketBuilder.h>

#include <vector>
#include <enet6/enet.h>

class LinkingContext
{
	public:
		LinkingContext(entt::registry& world, ComponentRegistry& components);
		~LinkingContext() = default;

		LinkingContext(const LinkingContext&) = delete;
		LinkingContext(LinkingContext&&) = delete;

		LinkingContext& operator=(const LinkingContext&) = delete;
		LinkingContext& operator=(LinkingContext&&) = delete;

		void ProcessPacket(ENetPeer* peer, ENetHost* host, const std::vector<uint8_t>& byteArray);

	private:
		entt::registry& mWorld;
		ComponentRegistry& mComponents;
		PacketBuilder mBuilder;
};

