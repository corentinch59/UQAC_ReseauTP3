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
	BinarySerializeType<int32_t>(byteArray, position.x);
	BinarySerializeType<int32_t>(byteArray, position.y);
	BinarySerializeType<int32_t>(byteArray, position.z);
	// Rotation
	BinarySerializeType<uint8_t>(byteArray, rotation.x * 100.f);
	BinarySerializeType<uint8_t>(byteArray, rotation.y * 100.f);
	BinarySerializeType<uint8_t>(byteArray, rotation.z * 100.f);
	BinarySerializeType<uint8_t>(byteArray, rotation.w * 100.f);
}

void TransformComponent::BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray)
{
	auto& node = handle.emplace<TransformComponent>();
	std::size_t offset = 0;
	// Position
	node.position.x = static_cast<float>(BinaryDeserialize<int32_t>(byteArray, offset));
	node.position.y = static_cast<float>(BinaryDeserialize<int32_t>(byteArray, offset));
	node.position.z = static_cast<float>(BinaryDeserialize<int32_t>(byteArray, offset));
	//Rotation
	node.rotation.x = static_cast<float>(BinaryDeserialize<int32_t>(byteArray, offset)) / 100.f;
	node.rotation.y = static_cast<float>(BinaryDeserialize<int32_t>(byteArray, offset)) / 100.f;
	node.rotation.z = static_cast<float>(BinaryDeserialize<int32_t>(byteArray, offset)) / 100.f;
	node.rotation.w = static_cast<float>(BinaryDeserialize<int32_t>(byteArray, offset)) / 100.f;
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
