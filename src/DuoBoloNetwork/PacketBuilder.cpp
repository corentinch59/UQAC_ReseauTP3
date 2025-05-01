#include <DuoBoloNetwork/PacketBuilder.h>
#include <DuoBoloNetwork/Opcodes.h>
#include <DuoBoloNetwork/Protocol.h>
#include <DuoBoloNetwork/BinarySerialize.h>

#include <DuoBoloShared/TransformComponent.h>
#include <DuoBoloShared/NetworkComponent.h>

#include <spdlog/spdlog.h>


ENetPacket* PacketBuilder::build_world_init_packet(entt::registry& world, ComponentRegistry& components)
{
	WorldInitPacket packet;
	packet.nbEntities = world.view<entt::entity>().size();
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

ENetPacket* PacketBuilder::build_entity_packet(const entt::handle& handle, ComponentRegistry& components)
{
	EntityPacket packet;
	int nbComponents = 0;
	components.ForEachComponent([&](const ComponentRegistry::Entry& entry)
		{
			if (entry.hasComponent(handle))
			{
				entry.binarySerialize(handle, packet.componentData);
				++nbComponents;
			}
		});
	packet.nbComp = nbComponents;
	
	return build_packet<EntityPacket>(packet, ENET_PACKET_FLAG_RELIABLE);
}

ENetPacket* PacketBuilder::build_game_data(entt::registry& world, ComponentRegistry& components)
{
	GameDataPacket packet;

	int count = 0;
	for (auto [entity, transform, network] : world.view<TransformComponent, NetworkComponent>().each())
	{
		entt::handle entityHandle(world, entity);
		if (transform.IsComponentModified(packet.worldArray))
		{
			network.BinarySerialize(entityHandle, packet.worldArray);
			count++;
		}
	}
	packet.nbEntities = count;

	return build_packet<GameDataPacket>(packet, 0);
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
