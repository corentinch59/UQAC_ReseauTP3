#pragma once

#include <DuoBoloNetwork/Opcodes.h>

#include <vector>

struct WorldInitPacket
{
	static constexpr Opcode opcode = Opcode::WorldInit;

	void Serialize(std::vector<uint8_t>& byteArray);
	static WorldInitPacket Deserialize(const std::vector<uint8_t>& byteArray, std::size_t& offset);
};