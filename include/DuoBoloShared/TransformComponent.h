//
// Created by theo on 12/03/2025.
//

#pragma once

#include <raylib.h>
#include <raymath.h>
#include <entt/entity/handle.hpp>
#include <nlohmann/json_fwd.hpp>
#include <cstdint>
#include <DuoBoloNetwork/Timer.h>

struct TransformComponent
{
    Vector3 position = {1, 1, 1};
    Quaternion rotation = QuaternionIdentity();
    Vector3 scale = { 1, 1, 1 }; // only affects rendering !!

    Vector3 previousPosition;
    Quaternion previousRotation;

	Vector3 targetPosition;
	Quaternion targetRotation;

    float interpolationTime = 0.1f; // 100 ms
    float interpElapsed = 0.0f;

    std::vector<uint8_t> previous;

    bool IsComponentModified(std::vector<uint8_t>& byteArray);

    void BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray);
    static void BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray, std::size_t& offset);

    nlohmann::json JsonSerialize(const entt::handle entity) const;
    static void JsonUnserialize(entt::handle entity, const nlohmann::json& doc);

};