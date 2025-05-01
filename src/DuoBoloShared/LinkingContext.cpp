#include <DuoBoloShared/LinkingContext.h>
#include <DuoBoloNetwork/BinarySerialize.h>
#include <DuoBoloNetwork/Opcodes.h>
#include <DuoBoloNetwork/Protocol.h>

#include <DuoBoloShared/TransformComponent.h>
#include <DuoBoloShared/RenderableComponent.h>
#include <DuoBoloShared/PhysicsComponent.h>

#include <spdlog/spdlog.h>

#include "DuoBoloShared/NetworkComponent.h"

LinkingContext::LinkingContext(entt::registry& world, ComponentRegistry& components) :
mWorld(world),
mComponents(components)
{
}

void LinkingContext::ProcessPacket(ENetPeer* peer, const std::vector<uint8_t>& byteArray)
{
	std::size_t offset = 0;
	Opcode opcode = static_cast<Opcode>(BinaryDeserialize<uint8_t>(byteArray, offset));
	switch (opcode)
	{
		case Opcode::WorldInit:
		{
			mWorld.clear();
			int nb = BinaryDeserialize<uint16_t>(byteArray, offset);
			for (int i = 0; i < nb; ++i)
			{
				entt::handle entityHandle(mWorld, mWorld.create());
				int nbComp = BinaryDeserialize<uint8_t>(byteArray, offset);
				for (int j = 0; j < nbComp; ++j)
				{
					ComponentType comp = static_cast<ComponentType>(BinaryDeserialize<uint8_t>(byteArray, offset));
					std::vector<ComponentRegistry::Entry>::iterator it;
					if (mComponents.FindEntry(comp, it))
					{
						it->binaryUnserialize(entityHandle, byteArray, offset);
					}
				}
			}
			break;
		}
		case Opcode::GameData:
		{
			int count = BinaryDeserialize<uint16_t>(byteArray, offset);

			std::unordered_map<std::uint32_t, entt::entity> id_entity_map;
			auto view = mWorld.view<TransformComponent, NetworkComponent>();
			for (auto&& [entity, transform, network] : view.each()) {
				id_entity_map[network.id] = entity;
			}

			for (std::size_t i = 0; i < count; ++i)
			{
				offset += sizeof(uint8_t);

				TransformComponent temp;
				temp.position.x = BinaryDeserialize<float>(byteArray, offset);
				temp.position.y = BinaryDeserialize<float>(byteArray, offset);
				temp.position.z = BinaryDeserialize<float>(byteArray, offset);

				temp.rotation.x = BinaryDeserialize<int8_t>(byteArray, offset) / 100.f;
				temp.rotation.y = BinaryDeserialize<int8_t>(byteArray, offset) / 100.f;
				temp.rotation.z = BinaryDeserialize<int8_t>(byteArray, offset) / 100.f;
				temp.rotation.w = BinaryDeserialize<int8_t>(byteArray, offset) / 100.f;

				temp.scale.x = BinaryDeserialize<float>(byteArray, offset);
				temp.scale.y = BinaryDeserialize<float>(byteArray, offset);
				temp.scale.z = BinaryDeserialize<float>(byteArray, offset);

				offset += sizeof(uint8_t);
				std::uint32_t id = BinaryDeserialize<std::uint32_t>(byteArray, offset);

				auto it = id_entity_map.find(id);
				if (it != id_entity_map.end()) {
					auto entity = it->second;
					auto& transform = view.get<TransformComponent>(entity);

					transform = temp;
				}
			}

			break;
		}
		case Opcode::Entity:
		{


			break;
		}
		case Opcode::Shoot:
		{
			ClientShootPacket packet = ClientShootPacket::Deserialize(byteArray, offset);

			auto sphereEntity = mWorld.create();
			auto& sphereTransform = mWorld.emplace<TransformComponent>(sphereEntity);
			sphereTransform.position = packet.position;
			sphereTransform.scale = { .2f , .2f , .2f };

			sphereTransform.rotation = QuaternionIdentity();

			auto& sphereRenderable = mWorld.emplace<RenderableComponent>(sphereEntity);
			sphereRenderable.model = "sphere";
			sphereRenderable.tint = RED;

			auto& sphereRigidbody = mWorld.emplace<RigidbodyComponent>(sphereEntity, 50.0f, SphereShape{ .2f });
			sphereRigidbody.velocity = Vector3Normalize(packet.forward) * 75.0f;

			auto& network = mWorld.emplace<NetworkComponent>(sphereEntity);
			network.id = static_cast<std::uint32_t>(sphereEntity);

			//Send the newly created entity
			
			break;
		}
	}
}
