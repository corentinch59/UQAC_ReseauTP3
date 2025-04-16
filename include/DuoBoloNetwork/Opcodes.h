#pragma once

#include <cstdint>

enum class Opcode : uint8_t
{
	WorldInit,
	GameData,
	Entity,
};

enum class ComponentTypeID : uint8_t
{
	Transform,
	Renderable,
};