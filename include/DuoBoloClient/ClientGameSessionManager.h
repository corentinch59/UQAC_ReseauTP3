#pragma once

#include <raylib.h>
#include <DuoBoloShared/INetworkEventListener.h>
#include <DuoBoloShared/ComponentRegistry.h>
#include <DuoBoloShared/LinkingContext.h>
#include <DuoBoloNetwork/PacketBuilder.h>
#include <DuoBoloNetwork/Input.h>

#include "OnlineClientManager.h"

class ClientGameSessionManager : public INetworkEventListener
{
	public:
		explicit ClientGameSessionManager(entt::registry& registry, ComponentRegistry& components, OnlineClientManager& onlineMgr);

		void OnConnected(ENetPeer* peer) override;
		void OnDisconnected(ENetPeer* peer) override;
		void OnTimedOut(ENetPeer* peer) override;
		void OnPacketReceived(ENetPeer* peer, const std::vector<uint8_t>& data) override;
		void Tick(ENetHost* host, float dt);

		void SendInputPacket(const PlayerInput& input);
		void SendShootPacket(Vector3 position, Vector3 forward);
	private:

		OnlineClientManager& mOnlineManager;
		LinkingContext mLinkingContext;
		PacketBuilder m_packetBuilder;
};

