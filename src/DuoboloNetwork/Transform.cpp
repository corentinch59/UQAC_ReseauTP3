#include <DuoBoloNetwork/Transform.h>
#include <DuoBoloNetwork/BinarySerializer.h>

#include <nlohmann/json.hpp>

void TransformComponent::BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray)
{
	// ID -|> Position -|> Rotation
	// ID
	BinarySerializer::BinarySerializeType(byteArray, "transform");
	// Position
	BinarySerializer::BinarySerializeType<float>(byteArray, position.x);
	BinarySerializer::BinarySerializeType<float>(byteArray, position.y);
	BinarySerializer::BinarySerializeType<float>(byteArray, position.z);
	// Rotation
	BinarySerializer::BinarySerializeType<float>(byteArray, rotation.x);
	BinarySerializer::BinarySerializeType<float>(byteArray, rotation.y);
	BinarySerializer::BinarySerializeType<float>(byteArray, rotation.z);
	BinarySerializer::BinarySerializeType<float>(byteArray, rotation.w);
}

void TransformComponent::BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray)
{
	
}

nlohmann::json TransformComponent::JsonSerialize(const entt::handle entity) const
{
	return nlohmann::json::object();
}

void TransformComponent::JsonUnserialize(entt::handle entity, const nlohmann::json& doc)
{
}
