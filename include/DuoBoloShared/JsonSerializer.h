#pragma once

#include <raylib.h>
#include <nlohmann/json.hpp>
#include <DuoBoloShared/PhysicsComponent.h>

inline void from_json(const nlohmann::json& j, Vector3& vec)
{
	vec.x = j.at("x");
	vec.y = j.at("y");
	vec.z = j.at("z");
}

inline void to_json(nlohmann::json& j, const Vector3& vec)
{
	j["x"] = vec.x;
	j["y"] = vec.y;
	j["z"] = vec.z;
}

inline void from_json(const nlohmann::json& j, Quaternion& quat)
{
	quat.x = j.at("x");
	quat.y = j.at("y");
	quat.z = j.at("z");
	quat.w = j.at("w");
}

inline void to_json(nlohmann::json& j, const Quaternion& quat)
{
	j["x"] = quat.x;
	j["y"] = quat.y;
	j["z"] = quat.z;
	j["w"] = quat.w;
}

inline void from_json(const nlohmann::json& j, Color& color)
{
	color.r = j.at("r");
	color.g = j.at("g");
	color.b = j.at("b");
	color.a = j.at("a");
}

inline void to_json(nlohmann::json& j, const Color& color)
{
	j["r"] = color.r;
	j["g"] = color.g;
	j["b"] = color.b;
	j["a"] = color.a;
}

inline void from_json(const nlohmann::json& j, BoxShape& shape)
{
	shape.extent = j.at("Extent");
}

inline void to_json(nlohmann::json& j, const BoxShape& shape)
{
	j["Extent"] = shape.extent;
}

inline void from_json(const nlohmann::json& j, SphereShape& shape)
{
	shape.radius = j.at("Radius");
}

inline void to_json(nlohmann::json& j, const SphereShape& shape)
{
	j["Radius"] = shape.radius;
}