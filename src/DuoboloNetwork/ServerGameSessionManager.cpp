#include <DuoBoloNetwork/ServerGameSessionManager.h>
#include <spdlog/spdlog.h>

ServerGameSessionManager::ServerGameSessionManager(entt::registry& world, ComponentRegistry& components) :
mWorld(world),
mComponents(components)
{
}

ServerGameSessionManager::~ServerGameSessionManager()
{
}

void ServerGameSessionManager::OnConnected(ENetPeer* peer)
{
	spdlog::info("Client connected, sending world info");
	enet_peer_send(peer, 0, mBuilder.build_world_init_packet(mWorld, mComponents));
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
}
