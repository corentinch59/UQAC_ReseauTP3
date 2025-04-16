//
// Created by theo on 12/03/2025.
//

#pragma once

#include <raylib.h>
#include <raymath.h>
#include <entt/entity/handle.hpp>
#include <nlohmann/json_fwd.hpp>

struct TransformComponent
{
    Vector3 position = {1, 1, 1};
    Quaternion rotation = QuaternionIdentity();
    Vector3 scale = { 1, 1, 1 }; // only affects rendering !!

    void BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray);
    static void BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray, std::size_t& offset);

    nlohmann::json JsonSerialize(const entt::handle entity) const;
    static void JsonUnserialize(entt::handle entity, const nlohmann::json& doc);

};