#include <DuoBoloNetwork/BinarySerialize.h>
#include <DuoBoloNetwork/Opcodes.h>
#include <DuoBoloShared/CameraComponent.h>
#include <DuoBoloShared/JsonSerializer.h>

#include <nlohmann/json.hpp>
#include <entt/entt.hpp>


void CameraComponent::BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray)
{
	
}

void CameraComponent::BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray, std::size_t& offset)
{

}

nlohmann::json CameraComponent::JsonSerialize(const entt::handle entity) const
{
	nlohmann::json doc;

	return doc;
}

void CameraComponent::JsonUnserialize(entt::handle entity, const nlohmann::json& doc)
{
	auto& node = entity.emplace<CameraComponent>();
}
