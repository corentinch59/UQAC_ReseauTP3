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
		std::size_t offset = packet.worldArray.size();
		BinarySerializeType<uint8_t>(packet.worldArray, 0);
		components.ForEachComponent([&](const ComponentRegistry::Entry& entry)
			{
				
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

ENetPacket* PacketBuilder::build_game_data(entt::registry& world, ComponentRegistry& components)
{
	WorldInitPacket packet;
	packet.nbEntities = world.view<entt::entity>().size();
	// Serialiser toutes les entites
	for (auto [entity] : world.storage<entt::entity>().each())
	{
		entt::handle entityHandle(world, entity);
		int nbComponents = 0;
		std::size_t offset = packet.worldArray.size();
		BinarySerializeType<uint8_t>(packet.worldArray, 0);
		components.ForEachComponent([&](const ComponentRegistry::Entry& entry)
			{

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

ENetPacket* PacketBuilder::build_client_input(const PlayerInput& inputs)
{
	ClientInputPacket packet;

	packet.input = inputs;

	return build_packet<ClientInputPacket>(packet, 0); 
}

ENetPacket* PacketBuilder::build_client_shoot(Vector3 position, Vector3 forward)
{
	ClientShootPacket packet;

	packet.position = position;
	packet.forward = forward;

	return build_packet<ClientShootPacket>(packet, 0);
}
