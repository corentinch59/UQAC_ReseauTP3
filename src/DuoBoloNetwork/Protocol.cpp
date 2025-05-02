#include <DuoBoloNetwork/Protocol.h>
#include <DuoBoloNetwork/BinarySerialize.h>

void WorldInitPacket::Serialize(std::vector<uint8_t>& byteArray) const
{
	BinarySerializeType<uint16_t>(byteArray, nbEntities);

	std::size_t oldSize = byteArray.size();
	byteArray.resize(sizeof(Opcode) + oldSize + worldArray.size());
	std::memcpy(&byteArray[sizeof(uint16_t) + sizeof(Opcode)], worldArray.data(), worldArray.size());
}

void GameDataPacket::Serialize(std::vector<std::uint8_t>& byteArray) const
{
	BinarySerializeType<uint16_t>(byteArray, nbEntities);

	std::size_t oldSize = byteArray.size();
	byteArray.resize(sizeof(Opcode) + oldSize + worldArray.size());
	std::memcpy(&byteArray[sizeof(uint16_t) + sizeof(Opcode)], worldArray.data(), worldArray.size());
}

void EntityPacket::Serialize(std::vector<std::uint8_t>& byteArray) const
{
	BinarySerializeType<uint8_t>(byteArray, nbComp);

	std::size_t oldSize = byteArray.size();
	byteArray.resize(sizeof(Opcode) + oldSize + componentData.size());
	std::memcpy(&byteArray[sizeof(uint8_t) + sizeof(Opcode)], componentData.data(), componentData.size());
}

EntityPacket EntityPacket::Deserialize(const std::vector<std::uint8_t>& byteArray, std::size_t& offset)
{
	return EntityPacket{};
}

void ClientInputPacket::Serialize(std::vector<std::uint8_t>& byteArray) const
{
	std::uint8_t byte = 0;
	if (input.forward)
		byte |= 1 << 0;

	if (input.backward)
		byte |= 1 << 1;

	if (input.left)
		byte |= 1 << 2;

	if (input.right)
		byte |= 1 << 3;

	if (input.up)
		byte |= 1 << 4;

	if (input.down)
		byte |= 1 << 5;

	if (input.shoot)
		byte |= 1 << 6;

	BinarySerializeType<std::uint8_t>(byteArray, byte);
}

ClientInputPacket ClientInputPacket::Deserialize(const std::vector<std::uint8_t>& byteArray, std::size_t& offset)
{
	ClientInputPacket packet;
	
	std::uint8_t inputByte = BinaryDeserialize<std::uint8_t>(byteArray, offset);

	packet.input.forward =	(inputByte & (1 << 0)) != 0;
	packet.input.backward = (inputByte & (1 << 1)) != 0;
	packet.input.left =		(inputByte & (1 << 2)) != 0;
	packet.input.right =	(inputByte & (1 << 3)) != 0;
	packet.input.up =		(inputByte & (1 << 4)) != 0;
	packet.input.down =		(inputByte & (1 << 5)) != 0;
	packet.input.shoot =	(inputByte & (1 << 6)) != 0;

	return packet;
}

void ClientShootPacket::Serialize(std::vector<std::uint8_t>& byteArray) const
{
	BinarySerializeType<float>(byteArray, position.x);
	BinarySerializeType<float>(byteArray, position.y);
	BinarySerializeType<float>(byteArray, position.z);

	BinarySerializeType<float>(byteArray, forward.x);
	BinarySerializeType<float>(byteArray, forward.y);
	BinarySerializeType<float>(byteArray, forward.z);
}

ClientShootPacket ClientShootPacket::Deserialize(const std::vector<std::uint8_t>& byteArray, std::size_t& offset)
{
	ClientShootPacket packet;

	packet.position.x = BinaryDeserialize<float>(byteArray, offset);
	packet.position.y = BinaryDeserialize<float>(byteArray, offset);
	packet.position.z = BinaryDeserialize<float>(byteArray, offset);

	packet.forward.x = BinaryDeserialize<float>(byteArray, offset);
	packet.forward.y = BinaryDeserialize<float>(byteArray, offset);
	packet.forward.z = BinaryDeserialize<float>(byteArray, offset);

	return packet;
}

void ClientAuthPacket::Serialize(std::vector<std::uint8_t>& byteArray) const
{
	BinarySerializeType(byteArray, username);
}

ClientAuthPacket ClientAuthPacket::Deserialize(const std::vector<std::uint8_t>& byteArray, std::size_t& offset)
{
	ClientAuthPacket packet;
	packet.username = BinaryDeserialize(byteArray, offset);

	return packet;
}
