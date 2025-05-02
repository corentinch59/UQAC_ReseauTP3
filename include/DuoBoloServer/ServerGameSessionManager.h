#pragma once

#include <cstdint>

#include <DuoBoloShared/INetworkEventListener.h>
#include <DuoBoloShared/ComponentRegistry.h>
#include <DuoBoloShared/LinkingContext.h>

#include <DuoBoloNetwork/PacketBuilder.h>

#include <DuoBoloServer/ServerGame.h>

class ServerGameSessionManager : public INetworkEventListener
{
	public:
		explicit ServerGameSessionManager(entt::registry& world, ComponentRegistry& components);

		~ServerGameSessionManager() override;

		void OnConnected(ENetPeer* peer) override;
		void OnDisconnected(ENetPeer* peer) override;
		void OnTimedOut(ENetPeer* peer) override;
		void OnPacketReceived(ENetPeer* peer, ENetHost* host, const std::vector<uint8_t>& data) override;

		void Tick(ENetHost* host, ServerGame& game, float dt);
		LinkingContext& GetContext() { return mLinkingContext; }

		std::string GetUsernameForConnection(uint32_t connectionId);

	private:
		entt::registry& mWorld;
		ComponentRegistry& mComponents;

		LinkingContext mLinkingContext;
		PacketBuilder mBuilder;
};

