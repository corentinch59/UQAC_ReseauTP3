#pragma once

#include <DuoboloNetwork/INetworkEventListener.h>
#include <DuoBoloShared/ComponentRegistry.h>
#include <DuoboloNetwork/PacketBuilder.h>

class ServerGameSessionManager : public INetworkEventListener
{
	public:
		explicit ServerGameSessionManager(entt::registry& world, ComponentRegistry& components);

		~ServerGameSessionManager() override;

		void OnConnected(ENetPeer* peer) override;
		void OnDisconnected(ENetPeer* peer) override;
		void OnTimedOut(ENetPeer* peer) override;
		void OnPacketReceived(ENetPeer* peer, const std::vector<uint8_t>& data) override;

	private:
		entt::registry& mWorld;
		ComponentRegistry& mComponents;

		PacketBuilder mBuilder;
};

