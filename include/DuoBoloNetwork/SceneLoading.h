#pragma once

#include <entt/fwd.hpp>

#include <string>

#define FILE_VERSION 1

class ComponentRegistry;

void LoadSceneFromPath(const std::string& path, entt::registry& world, const ComponentRegistry& componentRegistry);

void SaveSceneToPath(const std::string& path, entt::registry& world, const ComponentRegistry& componentRegistry);