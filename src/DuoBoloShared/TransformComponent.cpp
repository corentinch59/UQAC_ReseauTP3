#include <DuoBoloNetwork/BinarySerialize.h>
#include <DuoBoloNetwork/Opcodes.h>
#include <DuoBoloShared/TransformComponent.h>
#include <DuoBoloShared/JsonSerializer.h>

#include <nlohmann/json.hpp>

void TransformComponent::BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray)
{
	// ID -|> Position -|> Rotation
	// ID
	BinarySerializeType<uint8_t>(byteArray, static_cast<uint8_t>(ComponentTypeID::Transform));
	// Position
	BinarySerializeType<float>(byteArray, position.x);
	BinarySerializeType<float>(byteArray, position.y);
	BinarySerializeType<float>(byteArray, position.z);
	// Rotation
	BinarySerializeType<uint8_t>(byteArray, rotation.x * 100.f);
	BinarySerializeType<uint8_t>(byteArray, rotation.y * 100.f);
	BinarySerializeType<uint8_t>(byteArray, rotation.z * 100.f);
	BinarySerializeType<uint8_t>(byteArray, rotation.w * 100.f);
	// Scale
	BinarySerializeType<uint8_t>(byteArray, scale.x);
	BinarySerializeType<uint8_t>(byteArray, scale.y);
	BinarySerializeType<uint8_t>(byteArray, scale.z);
}

void TransformComponent::BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray, std::size_t& offset)
{
	auto& node = handle.emplace<TransformComponent>();
	// Position
	node.position.x = BinaryDeserialize<float>(byteArray, offset);
	node.position.y = BinaryDeserialize<float>(byteArray, offset);
	node.position.z = BinaryDeserialize<float>(byteArray, offset);
	// Rotation
	node.rotation.x = BinaryDeserialize<uint8_t>(byteArray, offset) / 100.f;
	node.rotation.y = BinaryDeserialize<uint8_t>(byteArray, offset) / 100.f;
	node.rotation.z = BinaryDeserialize<uint8_t>(byteArray, offset) / 100.f;
	node.rotation.w = BinaryDeserialize<uint8_t>(byteArray, offset) / 100.f;
	// Scale
	node.scale.x = BinaryDeserialize<uint8_t>(byteArray, offset);
	node.scale.y = BinaryDeserialize<uint8_t>(byteArray, offset);
	node.scale.z = BinaryDeserialize<uint8_t>(byteArray, offset);
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
