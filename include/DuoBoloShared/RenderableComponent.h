#pragma once

#include <raylib.h>
#include <entt/entt.hpp>
#include <nlohmann/json_fwd.hpp>

#include <string>

struct RenderableComponent
{
	std::string model = "cube";
	Color tint = WHITE;

	void BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray);
	static void BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray, std::size_t& offset);

	nlohmann::json JsonSerialize(const entt::handle entity) const;
	static void JsonUnserialize(entt::handle entity, const nlohmann::json& doc);
};
