#include <DuoBoloShared/NetworkComponent.h>
#include <DuoBoloNetwork/BinarySerialize.h>
#include <DuoBoloNetwork/Opcodes.h>

#include <nlohmann/json.hpp>

void NetworkComponent::BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray)
{
	BinarySerializeType<std::uint8_t>(byteArray, static_cast<uint8_t>(ComponentTypeID::Network));

	BinarySerializeType<std::uint32_t>(byteArray, id);
}

void NetworkComponent::BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray,
	std::size_t& offset)
{
	auto& node = handle.emplace<NetworkComponent>();
	node.id = BinaryDeserialize<std::uint32_t>(byteArray, offset);
}

nlohmann::json NetworkComponent::JsonSerialize(const entt::handle entity) const
{
	return nlohmann::json{};
}

void NetworkComponent::JsonUnserialize(entt::handle entity, const nlohmann::json& doc)
{

}
