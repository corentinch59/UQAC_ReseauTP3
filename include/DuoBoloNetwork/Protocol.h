#pragma once

#include <DuoBoloNetwork/Opcodes.h>
#include <DuoBoloNetwork/Input.h>

#include <vector>
#include <raylib.h>
#include <string>

struct WorldInitPacket
{
	static constexpr Opcode opcode = Opcode::WorldInit;

	int nbEntities;
	std::vector<std::uint8_t> worldArray;

	void Serialize(std::vector<std::uint8_t>& byteArray) const;
};

struct GameDataPacket
{
	static constexpr Opcode opcode = Opcode::GameData;

	int nbEntities;
	std::vector<std::uint8_t> worldArray;

	void Serialize(std::vector<std::uint8_t>& byteArray) const;
};

struct EntityPacket
{
	static constexpr Opcode opcode = Opcode::Entity;

	std::uint8_t nbComp;
	std::vector<std::uint8_t> componentData;

	void Serialize(std::vector<std::uint8_t>& byteArray) const;
	static EntityPacket Deserialize(const std::vector<std::uint8_t>& byteArray, std::size_t& offset);
};

struct ClientInputPacket
{
	static constexpr Opcode opcode = Opcode::Input;

	PlayerInput input;

	void Serialize(std::vector<std::uint8_t>& byteArray) const;
	static ClientInputPacket Deserialize(const std::vector<std::uint8_t>& byteArray, std::size_t& offset);
};

struct ClientShootPacket
{
	static constexpr Opcode opcode = Opcode::Shoot;

	Vector3 position;
	Vector3 forward;

	void Serialize(std::vector<std::uint8_t>& byteArray) const;
	static ClientShootPacket Deserialize(const std::vector<std::uint8_t>& byteArray, std::size_t& offset);
};

struct ClientAuthPacket
{
	static constexpr Opcode opcode = Opcode::Auth;

	std::string username;

	void Serialize(std::vector<std::uint8_t>& byteArray) const;
	static ClientAuthPacket Deserialize(const std::vector<std::uint8_t>& byteArray, std::size_t& offset);
};