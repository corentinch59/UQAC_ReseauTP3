#include <DuoBoloNetwork/PacketBuilder.h>
#include <DuoBoloNetwork/Opcodes.h>
#include <DuoBoloNetwork/Protocol.h>
#include <DuoBoloNetwork/BinarySerialize.h>
#include <spdlog/spdlog.h>

ENetPacket* PacketBuilder::build_world_init_packet(entt::registry& world, ComponentRegistry& components)
{
	WorldInitPacket packet;
	packet.nbEntities = world.view<entt::entity>().size();
	// Serialiser toutes les entites
	for(auto [entity] : world.storage<entt::entity>().each())
	{
		entt::handle entityHandle(world, entity);
		int nbComponents = 0;
		std::size_t offset = 0;
		components.ForEachComponent([&](const ComponentRegistry::Entry& entry)
			{
				offset = packet.worldArray.size();
				BinarySerializeType<uint8_t>(packet.worldArray, 0);
				if (entry.hasComponent(entityHandle))
				{
					entry.binarySerialize(entityHandle, packet.worldArray);
					++nbComponents;
				}
			});
		packet.worldArray[offset] = nbComponents;
	}

	return build_packet<WorldInitPacket>(packet, ENET_PACKET_FLAG_RELIABLE);
}

ENetPacket* PacketBuilder::build_player_packet()
{
	return nullptr;
}
