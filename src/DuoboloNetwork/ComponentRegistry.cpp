#include <DuoBoloNetwork/ComponentRegistry.h>

#include "DuoBoloNetwork/RenderableComponent.h"
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

	Register({
		.id = "renderable",
		.label = "Renderable",
		.addComponent = BuildAddComponent<RenderableComponent>(),
		.hasComponent = BuildHasComponent<RenderableComponent>(),
		.removeComponent = BuildRemoveComponent<RenderableComponent>(),
		.binarySerialize = BuildBinarySerialize<RenderableComponent>(),
		.binaryUnserialize = BuildBinaryUnserialize<RenderableComponent>(),
		.jsonSerialize = BuildJsonSerialize<RenderableComponent>(),
		.jsonUnserialize = BuildJsonUnserialize<RenderableComponent>()
		});
}
