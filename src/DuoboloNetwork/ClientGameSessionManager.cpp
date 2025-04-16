#include <DuoBoloNetwork/ClientGameSessionManager.h>
#include <DuoBoloNetwork/BinarySerialize.h>

#include <spdlog/spdlog.h>

#include "DuoBoloNetwork/Opcodes.h"

ClientGameSessionManager::ClientGameSessionManager(entt::registry& registry, ComponentRegistry& components) :
mWorld(registry),
mComponents(components)
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
	spdlog::info("Received {} bytes", data.size());
	std::size_t offset = 0;
	Opcode opcode = static_cast<Opcode>(BinaryDeserialize<uint8_t>(data, offset));
	switch(opcode)
	{
		case Opcode::WorldInit:
			{
				int nb = BinaryDeserialize<uint16_t>(data, offset);
				spdlog::info("There are {} entities", nb);
				for(int i = 0; i < nb; ++i)
				{
					entt::handle entityHandle(mWorld, mWorld.create());
					int nbComp = BinaryDeserialize<uint8_t>(data, offset);
					for(int j = 0; j < nbComp; ++j)
					{
						ComponentType comp = static_cast<ComponentType>(BinaryDeserialize<uint8_t>(data, offset));

					}
				}
				break;
			}
		case Opcode::GameData:
			{
				break;
			}
		case Opcode::Entity:
			{
				break;
			}
	}

}
