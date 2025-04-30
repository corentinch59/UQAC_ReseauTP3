#include <DuoBoloClient/ClientGameSessionManager.h>
#include <DuoBoloNetwork/BinarySerialize.h>
#include <DuoBoloNetwork/Opcodes.h>

#include <spdlog/spdlog.h>


ClientGameSessionManager::ClientGameSessionManager(entt::registry& registry, ComponentRegistry& components, OnlineClientManager& onlineMgr) :
mLinkingContext(registry, components),
mOnlineManager(onlineMgr)
{
}

void ClientGameSessionManager::OnConnected(ENetPeer* peer)
{

}

void ClientGameSessionManager::OnDisconnected(ENetPeer* peer)
{
}

void ClientGameSessionManager::OnTimedOut(ENetPeer* peer)
{
}

void ClientGameSessionManager::OnPacketReceived(ENetPeer* peer, const std::vector<uint8_t>& data)
{
	mLinkingContext.ProcessPacket(peer, data);
}

void ClientGameSessionManager::Tick(ENetHost* host, float dt)
{

}

void ClientGameSessionManager::SendInputPacket(const PlayerInput& input)
{
	ENetPacket* inputPacket = m_packetBuilder.build_client_input(input);
	mOnlineManager.SendPacket(inputPacket);
	enet_packet_dispose(inputPacket);
}

void ClientGameSessionManager::SendShootPacket(Vector3 position, Vector3 forward)
{
	ENetPacket* shootPacket = m_packetBuilder.build_client_shoot(position, forward);
	mOnlineManager.SendPacket(shootPacket);
	enet_packet_dispose(shootPacket);
}
