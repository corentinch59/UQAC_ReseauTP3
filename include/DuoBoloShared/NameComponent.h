#pragma once

#include <string>
#include <entt/entity/handle.hpp>
#include <vector>
#include <nlohmann/json_fwd.hpp>

struct NameComponent
{
	explicit NameComponent(std::string Name);

    void BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray);
    static void BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray);

    nlohmann::json JsonSerialize(const entt::handle entity) const;
    static void JsonUnserialize(entt::handle entity, const nlohmann::json& doc);

    std::string name;
};

