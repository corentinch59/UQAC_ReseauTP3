#pragma once

#include <DuoBoloNetwork/INetworkEventListener.h>
#include <DuoBoloShared/ComponentRegistry.h>
#include <DuoBoloNetwork/PacketBuilder.h>

class ServerGameSessionManager : public INetworkEventListener
{
	public:
		explicit ServerGameSessionManager(entt::registry& world, ComponentRegistry& components);

		~ServerGameSessionManager() override;

		void OnConnected(ENetPeer* peer) override;
		void OnDisconnected(ENetPeer* peer) override;
		void OnTimedOut(ENetPeer* peer) override;
		void OnPacketReceived(ENetPeer* peer, const std::vector<uint8_t>& data) override;

		void Tick(ENetHost* host, BaseGame& game, float dt) override;

	private:
		entt::registry& mWorld;
		ComponentRegistry& mComponents;

		PacketBuilder mBuilder;
};

