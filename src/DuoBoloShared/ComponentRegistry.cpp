#include <DuoBoloShared/ComponentRegistry.h>

#include <DuoBoloShared/RenderableComponent.h>
#include <DuoBoloShared/TransformComponent.h>
#include <DuoBoloShared/PhysicsComponent.h>
#include <DuoBoloShared/CameraComponent.h>
#include <DuoBoloShared/NameComponent.h>


ComponentRegistry::ComponentRegistry()
{
	RegisterEngineComponent();
}

void ComponentRegistry::ForEachComponent(const std::function<void(const Entry&)>& callback) const
{
	for(const auto& entry : mComponentTypes)
	{
		callback(entry);
	}
}

void ComponentRegistry::Register(Entry&& data)
{
	mComponentTypes.push_back(std::move(data));
}

void ComponentRegistry::RegisterEngineComponent()
{
	Register({
		.id = "transform",
		.label = "Transform",
		.addComponent = BuildAddComponent<TransformComponent>(),
		.hasComponent = BuildHasComponent<TransformComponent>(),
		.removeComponent = BuildRemoveComponent<TransformComponent>(),
		.binarySerialize = BuildBinarySerialize<TransformComponent>(),
		.binaryUnserialize = BuildBinaryUnserialize<TransformComponent>(),
		.jsonSerialize = BuildJsonSerialize<TransformComponent>(),
		.jsonUnserialize = BuildJsonUnserialize<TransformComponent>(),
#ifdef WITH_SCE_EDITOR
		.inspect = BuildInspect<TransformComponent>(),
#endif
		});

	Register({
		.id = "renderable",
		.label = "Renderable",
		.addComponent = BuildAddComponent<RenderableComponent>(),
		.hasComponent = BuildHasComponent<RenderableComponent>(),
		.removeComponent = BuildRemoveComponent<RenderableComponent>(),
		.binarySerialize = BuildBinarySerialize<RenderableComponent>(),
		.binaryUnserialize = BuildBinaryUnserialize<RenderableComponent>(),
		.jsonSerialize = BuildJsonSerialize<RenderableComponent>(),
		.jsonUnserialize = BuildJsonUnserialize<RenderableComponent>(),
		#ifdef WITH_SCE_EDITOR
		.inspect = BuildInspect<RenderableComponent>(),
#endif
		});

	Register({
		.id = "rigidBody",
		.label = "RigidBody",
		.addComponent = BuildAddComponent<RigidbodyComponent>(),
		.hasComponent = BuildHasComponent<RigidbodyComponent>(),
		.removeComponent = BuildRemoveComponent<RigidbodyComponent>(),
		.binarySerialize = BuildBinarySerialize<RigidbodyComponent>(),
		.binaryUnserialize = BuildBinaryUnserialize<RigidbodyComponent>(),
		.jsonSerialize = BuildJsonSerialize<RigidbodyComponent>(),
		.jsonUnserialize = BuildJsonUnserialize<RigidbodyComponent>(),
#ifdef WITH_SCE_EDITOR
		.inspect = BuildInspect<RigidbodyComponent>(),
#endif
		});

	Register({
		.id = "camera",
		.label = "Camera",
		.addComponent = BuildAddComponent<CameraComponent>(),
		.hasComponent = BuildHasComponent<CameraComponent>(),
		.removeComponent = BuildRemoveComponent<CameraComponent>(),
		.binarySerialize = BuildBinarySerialize<CameraComponent>(),
		.binaryUnserialize = BuildBinaryUnserialize<CameraComponent>(),
		.jsonSerialize = BuildJsonSerialize<CameraComponent>(),
		.jsonUnserialize = BuildJsonUnserialize<CameraComponent>()
		});

	Register({
		.id = "name",
		.label = "Name",
		.addComponent = [](entt::handle entity)
			{
				entity.emplace<NameComponent>("Unnamed");
			},
		.hasComponent = BuildHasComponent<NameComponent>(),
		.removeComponent = BuildRemoveComponent<NameComponent>(),
		.binarySerialize = BuildBinarySerialize<NameComponent>(),
		.binaryUnserialize = BuildBinaryUnserialize<NameComponent>(),
		.jsonSerialize = BuildJsonSerialize<NameComponent>(),
		.jsonUnserialize = BuildJsonUnserialize<NameComponent>(),
#ifdef WITH_SCE_EDITOR
		.inspect = BuildInspect<NameComponent>(),
#endif
		});
}
