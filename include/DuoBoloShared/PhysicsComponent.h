#pragma once

#include <entt/core/any.hpp>
#include <raylib.h>
#include <entt/entt.hpp>
#include <nlohmann/json_fwd.hpp>

struct BoxShape
{
    Vector3 extent;
};

struct SphereShape
{
    float radius;
};

struct RigidbodyComponent
{
    float mass = 1.f;
    entt::any shape = BoxShape{{1, 1, 1}};
    Vector3 velocity = {0, 0, 0};

    void BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray);
    static void BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray);

    nlohmann::json JsonSerialize(const entt::handle entity) const;
    static void JsonUnserialize(entt::handle entity, const nlohmann::json& doc);
};