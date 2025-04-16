#include <DuoBoloNetwork/Opcodes.h>
#include <DuoBoloNetwork/BinarySerialize.h>
#include <DuoBoloShared/RenderableComponent.h>
#include <DuoBoloShared/JsonSerializer.h>

#include <nlohmann/json.hpp>


void RenderableComponent::BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray)
{

	BinarySerializeType<uint8_t>(byteArray, static_cast<uint8_t>(ComponentTypeID::Renderable));

	BinarySerializeType(byteArray, model);

	BinarySerializeType<uint8_t>(byteArray, tint.r);
	BinarySerializeType<uint8_t>(byteArray, tint.g);
	BinarySerializeType<uint8_t>(byteArray, tint.b);
}

void RenderableComponent::BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray, std::size_t& offset)
{
	auto& node = handle.emplace<RenderableComponent>();

	node.model = BinaryDeserialize(byteArray, offset);

	node.tint.r = BinaryDeserialize<uint8_t>(byteArray, offset);
	node.tint.g = BinaryDeserialize<uint8_t>(byteArray, offset);
	node.tint.b = BinaryDeserialize<uint8_t>(byteArray, offset);
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
