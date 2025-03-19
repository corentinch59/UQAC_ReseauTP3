#pragma once

#include <raylib.h>
#include <entt/fwd.hpp>
#include <nlohmann/json_fwd.hpp>

struct CameraComponent
{
    float fovy = 60.f;
    int projection = CAMERA_PERSPECTIVE;
    bool isMainCamera = true;

    void BinarySerialize(entt::handle handle, std::vector<uint8_t>& byteArray);
    static void BinaryUnserialize(entt::handle handle, const std::vector<uint8_t>& byteArray);

    nlohmann::json JsonSerialize(const entt::handle entity) const;
    static void JsonUnserialize(entt::handle entity, const nlohmann::json& doc);
};
