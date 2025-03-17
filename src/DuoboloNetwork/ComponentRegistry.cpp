#include <DuoBoloNetwork/ComponentRegistry.h>

#include "DuoBoloNetwork/Transform.h"

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
		.jsonUnserialize = BuildJsonUnserialize<TransformComponent>()
		});
}
