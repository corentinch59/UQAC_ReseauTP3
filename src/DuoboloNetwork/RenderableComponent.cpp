#include <DuoBoloNetwork/RenderableComponent.h>
#include <DuoBoloNetwork/JsonSerializer.h>

#include <nlohmann/json.hpp>

void RenderableComponent::BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray)
{

}

void RenderableComponent::BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray)
{
}

nlohmann::json RenderableComponent::JsonSerialize(const entt::handle entity) const
{
	nlohmann::json doc;
	
	doc["Model"] = model;
	doc["Tint"] = tint;

	return doc;
}

void RenderableComponent::JsonUnserialize(entt::handle entity, const nlohmann::json& doc)
{
	auto& node = entity.emplace<RenderableComponent>();

	node.model = doc.value("Model", std::string{});
	node.tint = doc.value("Tint", WHITE);
}
