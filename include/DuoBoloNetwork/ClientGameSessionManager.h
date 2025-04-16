#pragma once

#include <DuoBoloNetwork/INetworkEventListener.h>
#include <DuoBoloShared/ComponentRegistry.h>
#include <DuoBoloNetwork/LinkingContext.h>
#include <DuoBoloNetwork/PacketBuilder.h>


class ClientGameSessionManager : public INetworkEventListener
{
	public:
		explicit ClientGameSessionManager(entt::registry& registry, ComponentRegistry& components);

		void OnConnected(ENetPeer* peer) override;
		void OnDisconnected(ENetPeer* peer) override;
		void OnTimedOut(ENetPeer* peer) override;
		void OnPacketReceived(ENetPeer* peer, const std::vector<uint8_t>& data) override;
	private:
		entt::registry& mWorld;
		ComponentRegistry& mComponents;

		LinkingContext mLinkingContext;
		PacketBuilder m_packetBuilder;
};

