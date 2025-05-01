#pragma once

#include <entt/entt.hpp>
#include <cstdint>
#include <nlohmann/json_fwd.hpp>

struct NetworkComponent
{
	std::uint32_t id;

	void BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray);
	static void BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray, std::size_t& offset);

	nlohmann::json JsonSerialize(const entt::handle entity) const;
	static void JsonUnserialize(entt::handle entity, const nlohmann::json& doc);
};

