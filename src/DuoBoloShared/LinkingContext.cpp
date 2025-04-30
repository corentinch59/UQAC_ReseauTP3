#include <DuoBoloShared/LinkingContext.h>
#include <DuoBoloNetwork/BinarySerialize.h>
#include <DuoBoloNetwork/Opcodes.h>
#include <DuoBoloNetwork/Protocol.h>

#include <DuoBoloShared/TransformComponent.h>
#include <DuoBoloShared/RenderableComponent.h>
#include <DuoBoloShared/PhysicsComponent.h>

#include <spdlog/spdlog.h>

LinkingContext::LinkingContext(entt::registry& world, ComponentRegistry& components) :
mWorld(world),
mComponents(components)
{
}

void LinkingContext::ProcessPacket(ENetPeer* peer, const std::vector<uint8_t>& byteArray)
{
	spdlog::info("Received {} bytes", byteArray.size());
	std::size_t offset = 0;
	Opcode opcode = static_cast<Opcode>(BinaryDeserialize<uint8_t>(byteArray, offset));
	switch (opcode)
	{
		case Opcode::WorldInit:
		{
			mWorld.clear();
			int nb = BinaryDeserialize<uint16_t>(byteArray, offset);
			//spdlog::info("There are {} entities", nb);
			for (int i = 0; i < nb; ++i)
			{
				entt::handle entityHandle(mWorld, mWorld.create());
				int nbComp = BinaryDeserialize<uint8_t>(byteArray, offset);
				//spdlog::info("{} components to deserialize", nbComp);
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
			mWorld.clear();
			int nb = BinaryDeserialize<uint16_t>(byteArray, offset);
			//spdlog::info("There are {} entities", nb);
			for (int i = 0; i < nb; ++i)
			{
				entt::handle entityHandle(mWorld, mWorld.create());
				int nbComp = BinaryDeserialize<uint8_t>(byteArray, offset);
				//spdlog::info("{} components to deserialize", nbComp);
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
		case Opcode::Entity:
		{
			break;
		}
		case Opcode::Shoot:
		{
			spdlog::info("Received shoot packet");
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
			
			break;
		}
	}
}
