#pragma once

#include <DuoBoloNetwork/Opcodes.h>

#include <vector>

struct WorldInitPacket
{
	static constexpr Opcode opcode = Opcode::WorldInit;

	int nbEntities;
	std::vector<uint8_t> worldArray;

	void Serialize(std::vector<uint8_t>& byteArray) const;
};

struct EntityPacket
{
	static constexpr Opcode opcode = Opcode::Entity;

	std::uint8_t nbComp;
	std::vector<uint8_t> componentData;

	void Serialize(std::vector<uint8_t>& byteArray) const;
	static EntityPacket Deserialize(const std::vector<uint8_t>& byteArray, std::size_t& offset);
};