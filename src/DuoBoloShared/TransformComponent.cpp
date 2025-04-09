#include <DuoBoloNetwork/BinarySerialize.h>
#include <DuoBoloShared/TransformComponent.h>
#include <DuoBoloShared/JsonSerializer.h>

#include <nlohmann/json.hpp>

void TransformComponent::BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray)
{
	// ID -|> Position -|> Rotation
	// ID
	BinarySerializeType(byteArray, "transform");
	// Position
	BinarySerializeType<float>(byteArray, position.x);
	BinarySerializeType<float>(byteArray, position.y);
	BinarySerializeType<float>(byteArray, position.z);
	// Rotation
	BinarySerializeType<float>(byteArray, rotation.x);
	BinarySerializeType<float>(byteArray, rotation.y);
	BinarySerializeType<float>(byteArray, rotation.z);
	BinarySerializeType<float>(byteArray, rotation.w);
}

void TransformComponent::BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray)
{

}

nlohmann::json TransformComponent::JsonSerialize(const entt::handle entity) const
{
	nlohmann::json doc;
	doc["Position"] = position;
	doc["Rotation"] = rotation;
	doc["Scale"] = scale;

	return doc;
}

void TransformComponent::JsonUnserialize(entt::handle entity, const nlohmann::json& doc)
{
	auto& node = entity.emplace<TransformComponent>();
	node.position = doc.value("Position", Vector3{0.f,0.f,0.f});
	node.rotation = doc.value("Rotation", Quaternion{0,0,0,1});
	node.scale = doc.value("Scale", Vector3{ 1.f,1.f,1.f });
}
