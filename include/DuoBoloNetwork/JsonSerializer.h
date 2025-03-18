#pragma once

#include <raylib.h>
#include <nlohmann/json.hpp>

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

inline void from_json(const nlohmann::json& j, Quaternion quat)
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