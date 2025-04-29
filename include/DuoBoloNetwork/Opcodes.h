#pragma once

#include <cstdint>

enum class Opcode : uint8_t
{
	WorldInit,
	GameData,
	Entity,
	Input,
	Shoot,
};

enum class ComponentTypeID : uint8_t
{
	Transform,
	Renderable,
	Camera,
	RigidBody,
	Name,
};