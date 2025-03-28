#include <DuoBoloShared/CameraComponent.h>
#include <DuoBoloShared/BinarySerializer.h>
#include <DuoBoloShared/JsonSerializer.h>

#include <nlohmann/json.hpp>
#include <entt/entt.hpp>

void CameraComponent::BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray)
{
	BinarySerializer::BinarySerializeType(byteArray, "camera");
	BinarySerializer::BinarySerializeType<float>(byteArray, fovy);
	BinarySerializer::BinarySerializeType<int>(byteArray, projection);
	BinarySerializer::BinarySerializeType<bool>(byteArray, isMainCamera);
}

void CameraComponent::BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray)
{

}

nlohmann::json CameraComponent::JsonSerialize(const entt::handle entity) const
{
	nlohmann::json doc;
	doc["FovY"] = fovy;
	doc["Projection"] = projection;
	doc["IsMainCamera"] = isMainCamera;

	return doc;
}

void CameraComponent::JsonUnserialize(entt::handle entity, const nlohmann::json& doc)
{
	auto& node = entity.emplace<CameraComponent>();
	node.fovy = doc.value("FovY", 60.0f);
	node.projection = doc.value("Projection", CAMERA_PERSPECTIVE);
	node.isMainCamera = doc.value("IsMainCamera", true);
}
