#include <DuoBoloNetwork/Protocol.h>
#include <DuoBoloNetwork/BinarySerialize.h>

void WorldInitPacket::Serialize(std::vector<uint8_t>& byteArray) const
{
	BinarySerializeType<uint16_t>(byteArray, nbEntities);

	std::size_t oldSize = byteArray.size();
	byteArray.resize(sizeof(Opcode) + oldSize + worldArray.size());
	std::memcpy(&byteArray[sizeof(uint16_t) + sizeof(Opcode)], worldArray.data(), worldArray.size());
}
