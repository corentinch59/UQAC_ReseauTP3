#include <DuoBoloServer/ServerGameSessionManager.h>
#include <spdlog/spdlog.h>

ServerGameSessionManager::ServerGameSessionManager(entt::registry& world, ComponentRegistry& components) :
mWorld(world),
mLinkingContext(world, components),
mComponents(components)
{
}

ServerGameSessionManager::~ServerGameSessionManager()
{
}

void ServerGameSessionManager::OnConnected(ENetPeer* peer)
{
	spdlog::info("Client connected, sending world info");
	ENetPacket* worldPacket = mBuilder.build_world_init_packet(mWorld, mComponents);
	enet_peer_send(peer, 0, worldPacket);
	enet_packet_dispose(worldPacket);
}

void ServerGameSessionManager::OnDisconnected(ENetPeer* peer)
{
	spdlog::info("Client {} disconnected", peer->incomingPeerID);
}

void ServerGameSessionManager::OnTimedOut(ENetPeer* peer)
{
	spdlog::info("Client {} timed out", peer->incomingPeerID);
}

void ServerGameSessionManager::OnPacketReceived(ENetPeer* peer, const std::vector<uint8_t>& data)
{
	mLinkingContext.ProcessPacket(peer, data);
}

void ServerGameSessionManager::Tick(ENetHost* host, ServerGame& game, float dt)
{
	if (host->connectedPeers > 0)
	{
		ENetPacket* packet = mBuilder.build_game_data(mWorld, mComponents);
		enet_host_broadcast(host, 0, packet);
		enet_packet_dispose(packet);
	}
}
