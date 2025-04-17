#include <DuoBoloNetwork/Opcodes.h>
#include <DuoBoloNetwork/BinarySerialize.h>
#include <DuoBoloShared/NameComponent.h>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>



NameComponent::NameComponent(std::string Name) :
name(std::move(Name))
{
}

void NameComponent::BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray)
{
	BinarySerializeType<uint8_t>(byteArray, static_cast<uint8_t>(ComponentTypeID::Name));
}

void NameComponent::BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray, std::size_t& offset)
{
}

nlohmann::json NameComponent::JsonSerialize(const entt::handle entity) const
{
	nlohmann::json doc;
	doc["Name"] = name;

	return doc;
}

void NameComponent::JsonUnserialize(entt::handle entity, const nlohmann::json& doc)
{
	entity.emplace<NameComponent>(doc.value("Name", ""));
}
